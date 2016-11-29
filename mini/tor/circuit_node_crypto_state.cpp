#include "circuit_node_crypto_state.h"
#include "relay_cell.h"

#include <mini/stack_buffer.h>
#include <mini/crypto/provider.h>
#include <mini/io/memory_stream.h>
#include <mini/io/stream_wrapper.h>

#include <mini/logger.h>

namespace mini::tor {

circuit_node_crypto_state::circuit_node_crypto_state(
  const byte_buffer_ref key_material
  )
{
  io::memory_stream key_material_stream(key_material);
  io::stream_wrapper key_material_buffer(key_material_stream);

  //
  // skip checksum digest.
  //
  stack_byte_buffer<20> checksum_digest;
  key_material_buffer.read(checksum_digest);

  stack_byte_buffer<20> df;
  key_material_buffer.read(df);
  _forward_digest.update(df);

  stack_byte_buffer<20> db;
  key_material_buffer.read(db);
  _backward_digest.update(db);

  stack_byte_buffer<16> kf;
  key_material_buffer.read(kf);
  _forward_cipher.init(aes128_t::key(kf));

  stack_byte_buffer<16> kb;
  key_material_buffer.read(kb);
  _backward_cipher.init(aes128_t::key(kb));
}

void
circuit_node_crypto_state::encrypt_forward_cell(
  relay_cell& cell
  )
{
  byte_buffer relay_payload_bytes(cell::payload_size);

  if (cell.get_payload().is_empty())
  {
    io::memory_stream relay_payload_stream(relay_payload_bytes);
    io::stream_wrapper relay_payload_buffer(relay_payload_stream, endianness::big_endian);

    relay_payload_buffer.write(cell.get_relay_command());
    relay_payload_buffer.write(static_cast<uint16_t>(0)); // 'recognized'
    relay_payload_buffer.write(cell.get_stream_id());
    relay_payload_buffer.write(static_cast<uint32_t>(0)); // digest placeholder
    relay_payload_buffer.write(static_cast<payload_size_type>(cell.get_relay_payload().get_size()));
    relay_payload_buffer.write(cell.get_relay_payload());

    //
    // update digest field in the payload
    //
    _forward_digest.update(relay_payload_bytes);
    auto digest = _forward_digest.duplicate().get();
    memory::copy(&relay_payload_bytes[5], &digest[0], sizeof(uint32_t));
  }
  else
  {
    relay_payload_bytes.insert_many(cell.get_payload(), 0);
  }

  //
  // encrypt the payload
  //
  auto encrypted_payload = _forward_cipher.update(relay_payload_bytes);
  mini_assert(encrypted_payload.get_size() == cell::payload_size);

  //
  // set the payload
  //
  cell.set_payload(encrypted_payload);
}

bool
circuit_node_crypto_state::decrypt_backward_cell(
  cell& cell
  )
{
  mini_assert(cell.get_payload().get_size() == cell::payload_size);
  auto decrypted_payload = _backward_cipher.update(cell.get_payload());

  mini_assert(decrypted_payload.get_size() == cell::payload_size);
  cell.set_payload(decrypted_payload);

  //
  // check if this is a cell for us.
  //
  if (cell.is_recognized())
  {
    //
    // remove the digest from the payload
    //
    byte_buffer payload_without_digest(cell.get_payload());
    memory::zero(payload_without_digest.get_buffer() + 5, sizeof(uint32_t));

    stack_byte_buffer<sizeof(uint32_t)> payload_digest;
    memory::copy(payload_digest.get_buffer(), cell.get_payload().get_buffer() + 5, sizeof(uint32_t));

    auto backward_digest_clone = _backward_digest.duplicate();
    backward_digest_clone.update(payload_without_digest);

    auto digest = backward_digest_clone.get();

    if (memory::equal(payload_digest.get_buffer(), &digest[0], sizeof(payload_digest)))
    {
      _backward_digest.update(payload_without_digest);

      return true;
    }
  }

  return false;
}

}
