#include "circuit_node.h"
#include "crypto/hybrid_encryption.h"

#include <mini/logger.h>
#include <mini/crypto/aes.h>
#include <mini/crypto/sha1.h>

namespace mini::tor {

static byte_buffer
derive_keys(
  const byte_buffer_ref secret
  )
{
  byte_buffer key_material;
  key_material.reserve(100);

  byte_buffer hashdata(secret);
  hashdata.resize(hashdata.get_size() + 1);

  for (uint8_t i = 0; i < 5; i++)
  {
    hashdata[secret.get_size()] = i;
    key_material.add_many(crypto::sha1::hash(hashdata));
  }

  return key_material;
}

circuit_node::circuit_node(
  circuit* circuit,
  onion_router* or,
  circuit_node_type node_type
  )
  : _circuit(circuit)
  , _type(node_type)
  , _onion_router(or)
  , _dh(node_type == circuit_node_type::introduction_point ? 128 : DH_SEC_LEN)
{

}

circuit*
circuit_node::get_circuit(
  void
  )
{
  return _circuit;
}

onion_router*
circuit_node::get_onion_router(
  void
  )
{
  return _onion_router;
}

key_agreement&
circuit_node::get_key_agreement(
  void
  )
{
  return _dh;
}

byte_buffer
circuit_node::create_onion_skin(
  void
  )
{
  return hybrid_encryption::encrypt(
    _dh.get_public_key(),
    _onion_router->get_onion_key()
    );
}

void
circuit_node::set_shared_secret(
  const byte_buffer_ref peer_public,
  const byte_buffer_ref kh // derivative key data, for verification of derivation
  )
{
  mini_assert(kh.get_size() == HASH_LEN)

  auto shared_secret = _dh.get_shared_secret(peer_public);
  auto key_material = derive_keys(shared_secret);

  if (memory::equal(key_material.get_buffer(), kh.get_buffer(), kh.get_size()))
  {
    _crypto_state = new circuit_node_crypto_state(key_material);
  }
}

bool
circuit_node::has_valid_crypto_state(
  void
  ) const
{
  return _crypto_state != nullptr;
}

void
circuit_node::encrypt_forward_cell(
  relay_cell& cell
  )
{
  _crypto_state->encrypt_forward_cell(cell);
}

bool
circuit_node::decrypt_backward_cell(
  cell& cell
  )
{
  return _crypto_state->decrypt_backward_cell(cell);
}

void
circuit_node::decrement_package_window(
  void
  )
{
  //
  // called when a relay data cell has been sent (on this circuit node).
  //
  mini_lock(_window_mutex)
  {
    _package_window--;

    mini_debug("circuit_node::decrement_package_window() [ _package_window = %u ]", _package_window);
  }
}

void
circuit_node::increment_package_window(
  void
  )
{
  //
  // called when a RELAY_SENDME with stream_id == 0 has been received.
  //
  mini_lock(_window_mutex)
  {
    _package_window += window_increment;

    mini_debug("circuit_node::increment_package_window() [ _package_window = %u ]", _package_window);
  }
}

void
circuit_node::decrement_deliver_window(
  void
  )
{
  //
  // called when a relay data cell has been received (on this circuit node).
  //
  mini_lock(_window_mutex)
  {
    _deliver_window--;

    mini_debug("circuit_node::decrement_deliver_window() [ _deliver_window = %u ]", _deliver_window);
  }
}

bool
circuit_node::consider_sending_sendme(
  void
  )
{
  mini_lock(_window_mutex)
  {
    if (_deliver_window > (window_start - window_increment))
    {
      mini_debug("circuit_node::consider_sending_sendme(): false");
      return false;
    }

    _deliver_window += window_increment;

    mini_debug("circuit_node::consider_sending_sendme(): true");
    return true;
  }
}

}
