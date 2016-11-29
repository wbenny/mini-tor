#pragma once
#include "cell.h"

#include <mini/byte_buffer.h>
#include <mini/crypto/aes.h>
#include <mini/crypto/sha1.h>

namespace mini::tor {

//
// sha1 hash size.
//
static constexpr size_type HASH_LEN = 20;

class relay_cell;

class circuit_node_crypto_state
{
  MINI_MAKE_NONCOPYABLE(circuit_node_crypto_state);

  public:
    circuit_node_crypto_state(
      const byte_buffer_ref key_material
      );

    ~circuit_node_crypto_state(
      void
      ) = default;

    void
    encrypt_forward_cell(
      relay_cell& cell
      );

    bool
    decrypt_backward_cell(
      cell& cell
      );

  private:
    using aes128_t = crypto::aes_ctr<128>;

    aes128_t _forward_cipher;
    aes128_t _backward_cipher;

    crypto::sha1 _forward_digest;
    crypto::sha1 _backward_digest;
};

}
