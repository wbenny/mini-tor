#pragma once
#include "common.h"
#include "cng/curve25519.h"
#include "ext/curve25519.h"

namespace mini::crypto {

using curve25519 = MINI_CRYPTO_CURVE25519_NAMESPACE::curve25519;
// using curve25519 = ext::curve25519;

}
