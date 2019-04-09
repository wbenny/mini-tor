#pragma once
#include "../common.h"

#include <mini/common.h>

#include <windows.h>
#include <bcrypt.h>

namespace mini::crypto::cng {

class provider
{
  MINI_MAKE_NONCOPYABLE(provider);

  public:
    provider(
      void
      );

    ~provider(
      void
      );

    template <
      cipher_mode AES_MODE
    >
    BCRYPT_ALG_HANDLE
    get_aes_handle(
      void
      )
    {
      return get_aes_handle(AES_MODE);
    }

    BCRYPT_ALG_HANDLE
    get_aes_handle(
      cipher_mode mode
      );

    BCRYPT_ALG_HANDLE
    get_rsa_handle(
      void
      );

    BCRYPT_ALG_HANDLE
    get_dh_handle(
      void
      );

    BCRYPT_ALG_HANDLE
    get_curve25519_handle(
      void
      );

    BCRYPT_ALG_HANDLE
    get_rng_handle(
      void
      );

    BCRYPT_ALG_HANDLE
    get_hash_sha1_handle(
      void
      );

    BCRYPT_ALG_HANDLE
    get_hash_hmac_sha256_handle(
      void
      );

  private:
    void
    init(
      void
      );

    void
    destroy(
      void
      );

    void
    init_win10(
      void
      );

    void
    destroy_win10(
      void
      );

    BCRYPT_ALG_HANDLE _provider_aes[static_cast<int>(cipher_mode::max)];
    BCRYPT_ALG_HANDLE _provider_rsa = nullptr;
    BCRYPT_ALG_HANDLE _provider_dh = nullptr;
    BCRYPT_ALG_HANDLE _provider_curve25519 = nullptr;
    BCRYPT_ALG_HANDLE _provider_rng = nullptr;
    BCRYPT_ALG_HANDLE _provider_hash_sha1 = nullptr;
    BCRYPT_ALG_HANDLE _provider_hash_hmac_sha256 = nullptr;
};

extern provider provider_factory;

}
