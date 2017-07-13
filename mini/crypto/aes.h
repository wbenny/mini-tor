#pragma once
#include "common.h"
#include "capi/aes.h"
#include "cng/aes.h"

namespace mini::crypto {

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
using aes_key = MINI_CRYPTO_AES_NAMESPACE::aes_key<AES_MODE, KEY_SIZE>;

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
using aes = MINI_CRYPTO_AES_NAMESPACE::aes<AES_MODE, KEY_SIZE>;

}
