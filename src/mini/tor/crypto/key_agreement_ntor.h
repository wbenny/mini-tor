#pragma once
#include "key_agreement.h"

#include <mini/crypto/curve25519.h>

namespace mini::tor {

class key_agreement_ntor
  : public key_agreement
{
  public:
    key_agreement_ntor(
      onion_router* router
      );

    key_agreement_ntor(
      onion_router* router,
      crypto::curve25519::private_key&& private_key
      );

    byte_buffer_ref
    get_public_key(
      void
      ) const override;

    byte_buffer_ref
    get_private_key(
      void
      ) const override;

    byte_buffer
    compute_shared_secret(
      const byte_buffer_ref handshake_data
      ) override;

    byte_buffer
    compute_shared_secret(
      const byte_buffer_ref other_public_key,
      const byte_buffer_ref derivative_key_data
      ) override;

  private:
    crypto::curve25519::private_key _private_key;
};

}
