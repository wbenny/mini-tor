#pragma once
#include "cell.h"

#include <mini/byte_buffer.h>
#include <mini/crypto/aes.h>
#include <mini/crypto/sha1.h>

namespace mini::tor {

//
// sha1 hash size.
//
static constexpr size_t HASH_LEN = 20;

class relay_cell;

class circuit_node_crypto_state
{
  public:
    MINI_MAKE_NONCOPYABLE(circuit_node_crypto_state);

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
    ptr<crypto::aes> _forward_cipher;
    ptr<crypto::aes> _backward_cipher;

    ptr<crypto::sha1> _forward_digest;
    ptr<crypto::sha1> _backward_digest;
};

}
