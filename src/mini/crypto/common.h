#pragma once
#include <mini/common.h>

//
// configuration of crypto classes.
// in [] is noted which namespaces are currently supported.w
//
// unless stated otherwise, capi & cng are supported on win7+.
//

//
// base[N] encoding/decoding
//
#ifndef MINI_CRYPTO_BASE16_NAMESPACE
#define MINI_CRYPTO_BASE16_NAMESPACE      capi  // [capi]
#endif

#ifndef MINI_CRYPTO_BASE32_NAMESPACE
#define MINI_CRYPTO_BASE32_NAMESPACE      ext   // [ext]
#endif

#ifndef MINI_CRYPTO_BASE64_NAMESPACE
#define MINI_CRYPTO_BASE64_NAMESPACE      capi  // [capi]
#endif

//
// symmetric crypto
//
#ifndef MINI_CRYPTO_AES_NAMESPACE
#define MINI_CRYPTO_AES_NAMESPACE         cng   // [capi, cng]
#endif

//
// asymmetric crypto
//
#ifndef MINI_CRYPTO_CURVE25519_NAMESPACE
#define MINI_CRYPTO_CURVE25519_NAMESPACE  cng   // [cng(win10+), ext]
#endif

#ifndef MINI_CRYPTO_DH_NAMESPACE
#define MINI_CRYPTO_DH_NAMESPACE          cng   // [capi, cng(win8.1+)]
#endif

#ifndef MINI_CRYPTO_RSA_NAMESPACE
#define MINI_CRYPTO_RSA_NAMESPACE         cng   // [capi, cng]
#endif

//
// hash/hmac
//
#ifndef MINI_CRYPTO_HASH_NAMESPACE
#define MINI_CRYPTO_HASH_NAMESPACE        cng   // [capi, cng]
#endif

#ifndef MINI_CRYPTO_HMAC_NAMESPACE
#define MINI_CRYPTO_HMAC_NAMESPACE        cng   // [capi, cng]
#endif

//
// random device
//
#ifndef MINI_CRYPTO_RANDOM_NAMESPACE
#define MINI_CRYPTO_RANDOM_NAMESPACE      cng   // [capi, cng]
#endif

namespace mini::crypto {

//
// supported cipher modes for block encryption algorithms.
//
enum class cipher_mode
{
  cbc,
  ecb,
  ofb,
  cfb,
  cts,
  ctr,

  max,
};

//
// supported RSA encryption padding schemes.
//
enum class rsa_encryption_padding
{
  pkcs1,
  oaep_sha1,

  max,
};

//
// supported hash algorithms.
//
enum class hash_algorithm_type
{
  md5,
  sha1,
  sha256,
  sha512,

  max,
};

static constexpr size_type
hash_algorithm_to_bit_size(
  hash_algorithm_type hash_algorithm
  )
{
  return
    hash_algorithm == hash_algorithm_type::md5    ? 128 :
    hash_algorithm == hash_algorithm_type::sha1   ? 160 :
    hash_algorithm == hash_algorithm_type::sha256 ? 256 :
    hash_algorithm == hash_algorithm_type::sha512 ? 512 :
    0;
}

}
