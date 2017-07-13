#pragma once
#include "common.h"
#include "capi/random.h"
#include "cng/random.h"

namespace mini::crypto {

using random = MINI_CRYPTO_RANDOM_NAMESPACE::random;

//
// TODO:
// it would be great to come up with better
// solution than this.
//
static auto& random_device = MINI_CRYPTO_RANDOM_NAMESPACE::random_device;

}
