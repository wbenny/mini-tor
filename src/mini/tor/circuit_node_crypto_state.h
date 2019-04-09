#pragma once
#include "cell.h"

#include <mini/byte_buffer.h>
#include <mini/crypto/aes.h>
#include <mini/crypto/sha1.h>

namespace mini::tor {

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
    using aes_ctr_128 = crypto::aes<crypto::cipher_mode::ctr, 128>;

    aes_ctr_128 _forward_cipher;
    aes_ctr_128 _backward_cipher;

    crypto::sha1 _forward_digest;
    crypto::sha1 _backward_digest;
};

}
