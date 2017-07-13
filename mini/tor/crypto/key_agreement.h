#pragma once
#include "../onion_router.h"

#include <mini/byte_buffer.h>

namespace mini::tor {

class key_agreement
{
  public:
    key_agreement(
      onion_router* router
      )
      : _onion_router(router)
    {

    }

    virtual ~key_agreement(
      void
      ) = default;

    virtual byte_buffer_ref
    get_public_key(
      void
      ) const = 0;

    virtual byte_buffer_ref
    get_private_key(
      void
      ) const = 0;

    virtual byte_buffer
    compute_shared_secret(
      const byte_buffer_ref handshake_data
      ) = 0;

    virtual byte_buffer
    compute_shared_secret(
      const byte_buffer_ref other_public_key,
      const byte_buffer_ref verification_data // derivative key data (KH), for verification of derivation
      ) = 0;

  protected:
    onion_router* _onion_router;
};

}
