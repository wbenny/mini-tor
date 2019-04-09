#include "provider.h"

// #define MINI_CRYPTO_CNG_PROVIDER_WIN10

namespace mini::crypto::cng {

provider provider_factory;

provider::provider(
  void
  )
{
#if defined(MINI_CRYPTO_CNG_PROVIDER_WIN10)
  init_win10();
#else
  init();
#endif
}

provider::~provider(
  void
  )
{
#if defined(MINI_CRYPTO_CNG_PROVIDER_WIN10)
  destroy_win10();
#else
  destroy();
#endif
}

BCRYPT_ALG_HANDLE
provider::get_aes_handle(
  cipher_mode mode
  )
{
  return _provider_aes[static_cast<int>(mode)];
}

BCRYPT_ALG_HANDLE
provider::get_rsa_handle(
  void
  )
{
  return _provider_rsa;
}

BCRYPT_ALG_HANDLE
provider::get_dh_handle(
  void
  )
{
  return _provider_dh;
}

BCRYPT_ALG_HANDLE
provider::get_curve25519_handle(
  void
  )
{
  return _provider_curve25519;
}

BCRYPT_ALG_HANDLE
provider::get_rng_handle(
  void
  )
{
  return _provider_rng;
}

BCRYPT_ALG_HANDLE
provider::get_hash_sha1_handle(
  void
  )
{
  return _provider_hash_sha1;
}

BCRYPT_ALG_HANDLE
provider::get_hash_hmac_sha256_handle(
  void
  )
{
  return _provider_hash_hmac_sha256;
}

void
provider::init(
  void
  )
{
  //
  // alternative provider
  //
  if (GetEnvironmentVariable(
    TEXT("bcrypt_alt_prov"),
    nullptr,
    0))
  {
    LoadLibrary(TEXT("bcrypt_alt.dll"));
  }

  //
  // aes
  //
  BCryptOpenAlgorithmProvider(
    &_provider_aes[static_cast<int>(cipher_mode::cbc)],
    BCRYPT_AES_ALGORITHM,
    NULL,
    0);

  BCryptSetProperty(
    _provider_aes[static_cast<int>(cipher_mode::cbc)],
    BCRYPT_CHAINING_MODE,
    (PBYTE)BCRYPT_CHAIN_MODE_CBC,
    sizeof(BCRYPT_CHAIN_MODE_CBC),
    0);

  BCryptOpenAlgorithmProvider(
    &_provider_aes[static_cast<int>(cipher_mode::ecb)],
    BCRYPT_AES_ALGORITHM,
    NULL,
    0);

  BCryptSetProperty(
    _provider_aes[static_cast<int>(cipher_mode::ecb)],
    BCRYPT_CHAINING_MODE,
    (PBYTE)BCRYPT_CHAIN_MODE_ECB,
    sizeof(BCRYPT_CHAIN_MODE_ECB),
    0);

  _provider_aes[static_cast<int>(cipher_mode::ofb)] = nullptr;
  _provider_aes[static_cast<int>(cipher_mode::cfb)] = nullptr;
  _provider_aes[static_cast<int>(cipher_mode::cts)] = nullptr;
  _provider_aes[static_cast<int>(cipher_mode::ctr)] = _provider_aes[static_cast<int>(cipher_mode::ecb)];

  //
  // rsa
  //
  BCryptOpenAlgorithmProvider(
    &_provider_rsa,
    BCRYPT_RSA_ALGORITHM,
    NULL,
    0);

  //
  // dh
  //
  BCryptOpenAlgorithmProvider(
    &_provider_dh,
    BCRYPT_DH_ALGORITHM,
    NULL,
    0);

  //
  // curve25519
  //
  BCryptOpenAlgorithmProvider(
    &_provider_curve25519,
    BCRYPT_ECDH_ALGORITHM,
    NULL,
    0);

  BCryptSetProperty(
    _provider_curve25519,
    BCRYPT_ECC_CURVE_NAME,
    (PBYTE)BCRYPT_ECC_CURVE_25519,
    sizeof(BCRYPT_ECC_CURVE_25519),
    0);

  //
  // rng
  //
  BCryptOpenAlgorithmProvider(
    &_provider_rng,
    BCRYPT_RNG_ALGORITHM,
    NULL,
    0);

  //
  // sha1
  //
  BCryptOpenAlgorithmProvider(
    &_provider_hash_sha1,
    BCRYPT_SHA1_ALGORITHM,
    NULL,
    0);

  //
  // hmac sha256
  //
  BCryptOpenAlgorithmProvider(
    &_provider_hash_hmac_sha256,
    BCRYPT_SHA256_ALGORITHM,
    NULL,
    BCRYPT_ALG_HANDLE_HMAC_FLAG);
}

void
provider::destroy(
  void
  )
{
  if (_provider_hash_hmac_sha256)
  {
    BCryptCloseAlgorithmProvider(
      _provider_hash_hmac_sha256,
      0);
  }

  if (_provider_hash_sha1)
  {
    BCryptCloseAlgorithmProvider(
      _provider_hash_sha1,
      0);
  }

  if (_provider_rng)
  {
    BCryptCloseAlgorithmProvider(
      _provider_rng,
      0);
  }

  if (_provider_curve25519)
  {
    BCryptCloseAlgorithmProvider(
      _provider_curve25519,
      0);
  }

  if (_provider_dh)
  {
    BCryptCloseAlgorithmProvider(
      _provider_dh,
      0);
  }

  if (_provider_rsa)
  {
    BCryptCloseAlgorithmProvider(
      _provider_rsa,
      0);
  }

  for (auto provider_handle : _provider_aes)
  {
    if (provider_handle)
    {
      BCryptCloseAlgorithmProvider(
        provider_handle,
        0);
    }
  }
}

void
provider::init_win10(
  void
  )
{
  //
  // ""
  //    beginning in Windows 10, CNG provides pre-defined algorithm handles for many algorithms.
  //    you can use these handles in any situation that requires an algorithm handle.
  //                                                                                  ""
  //
  // ref: https://msdn.microsoft.com/en-us/library/windows/desktop/mt631337(v=vs.85).aspx
  //

  _provider_aes[static_cast<int>(cipher_mode::cbc)] = BCRYPT_AES_CBC_ALG_HANDLE;
  _provider_aes[static_cast<int>(cipher_mode::ecb)] = BCRYPT_AES_ECB_ALG_HANDLE;
  _provider_aes[static_cast<int>(cipher_mode::ofb)] = nullptr;
  _provider_aes[static_cast<int>(cipher_mode::cfb)] = nullptr;
  _provider_aes[static_cast<int>(cipher_mode::cts)] = nullptr;
  _provider_aes[static_cast<int>(cipher_mode::ctr)] = BCRYPT_AES_ECB_ALG_HANDLE;

  _provider_rsa              = BCRYPT_RSA_ALG_HANDLE;
  _provider_dh               = BCRYPT_DH_ALG_HANDLE;
//_provider_curve25519
  _provider_rng              = BCRYPT_RNG_ALG_HANDLE;
  _provider_hash_sha1        = BCRYPT_SHA1_ALG_HANDLE;
  _provider_hash_hmac_sha256 = BCRYPT_HMAC_SHA256_ALG_HANDLE;

  //
  // curve25519
  //
  BCryptOpenAlgorithmProvider(
    &_provider_curve25519,
    BCRYPT_ECDH_ALGORITHM,
    NULL,
    0);

  BCryptSetProperty(
    _provider_curve25519,
    BCRYPT_ECC_CURVE_NAME,
    (PBYTE)BCRYPT_ECC_CURVE_25519,
    sizeof(BCRYPT_ECC_CURVE_25519),
    0);

}

void
provider::destroy_win10(
  void
  )
{
  if (_provider_curve25519)
  {
    BCryptCloseAlgorithmProvider(
      _provider_curve25519,
      0);
  }
}


}
