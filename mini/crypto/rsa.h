#pragma once
#include "common.h"
#include "capi/rsa.h"
#include "cng/rsa.h"

namespace mini::crypto {

template <
  size_type KEY_SIZE
>
using rsa = MINI_CRYPTO_RSA_NAMESPACE::rsa<KEY_SIZE>;

template <
  size_type KEY_SIZE
>
using rsa_public_key = MINI_CRYPTO_RSA_NAMESPACE::rsa_public_key<KEY_SIZE>;

template <
  size_type KEY_SIZE
>
using rsa_private_key = MINI_CRYPTO_RSA_NAMESPACE::rsa_private_key<KEY_SIZE>;


}
