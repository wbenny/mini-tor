#pragma once
#include "common.h"
#include "capi/dh.h"
#include "cng/dh.h"

namespace mini::crypto {

template <
  size_type KEY_SIZE
>
using dh = MINI_CRYPTO_DH_NAMESPACE::dh<KEY_SIZE>;

template <
  size_type KEY_SIZE
>
using dh_public_key = MINI_CRYPTO_DH_NAMESPACE::dh_public_key<KEY_SIZE>;

template <
  size_type KEY_SIZE
>
using dh_private_key = MINI_CRYPTO_DH_NAMESPACE::dh_private_key<KEY_SIZE>;


}
