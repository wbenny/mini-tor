#include "circuit_node.h"
#include "crypto/hybrid_encryption.h"
#include "crypto/key_agreement_tap.h"
#include "crypto/key_agreement_ntor.h"

#include <mini/logger.h>

#include <mini/crypto/base16.h>

namespace mini::tor {

circuit_node::circuit_node(
  circuit* circuit,
  onion_router* router,
  circuit_node_type node_type
  )
  : _circuit(circuit)
  , _type(node_type)
  , _onion_router(router)
{
  if (_type == circuit_node_type::introduction_point)
  {
    _handshake = new key_agreement_tap(_onion_router);
  }
}

circuit*
circuit_node::get_circuit(
  void
  )
{
  return _circuit;
}

circuit_node_type
circuit_node::get_circuit_node_type(
  void
  ) const
{
  return _type;
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
  return *_handshake;
}

byte_buffer
circuit_node::create_onion_skin(
  void
  )
{
  _handshake = new key_agreement_tap(_onion_router);

  return hybrid_encryption::encrypt(
    _handshake->get_public_key(),
    _onion_router->get_onion_key()
    );
}

byte_buffer
circuit_node::create_onion_skin_ntor(
  void
  )
{
  _handshake = new key_agreement_ntor(_onion_router);

  return {
    _onion_router->get_identity_fingerprint(),
    _onion_router->get_ntor_onion_key(),
    _handshake->get_public_key()
  };
}

void
circuit_node::compute_shared_secret(
  const byte_buffer_ref cell_payload
  )
{
  auto key_material = _handshake->compute_shared_secret(cell_payload);

  if (!key_material.is_empty())
  {
    //
    // struct key_material
    // {
    //   byte_type digest_forward [20];
    //   byte_type digest_backward[20];
    //   byte_type cipher_forward [16];
    //   byte_type cipher_backward[16];
    //   ^^ sizeof == 40 + 32 == 72 ^^
    //
    //   byte_type rend_nonce     [20]; << ignored now
    //   ^^ sizeof == 72 + 20 == 92 ^^  << (used in establishing of introduction points,
    //                                      rend-spec.txt § 1.2.)
    //
    //   byte_type __garbage__    [];   << ignored
    // };
    //

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
