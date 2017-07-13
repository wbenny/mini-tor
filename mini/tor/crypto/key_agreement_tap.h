#pragma once
#include "key_agreement.h"

#include <mini/crypto/dh.h>

namespace mini::tor {

class key_agreement_tap
  : public key_agreement
{
  public:
    using dh1024 = crypto::dh<1024>;

    key_agreement_tap(
      onion_router* router
      );

    key_agreement_tap(
      onion_router* router,
      dh1024::private_key&& private_key
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
      const byte_buffer_ref verification_data
      ) override;

  private:
    dh1024::private_key _private_key;
};

}
