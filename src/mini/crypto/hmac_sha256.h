#pragma once
#include "common.h"
#include "capi/hmac.h"
#include "cng/hmac.h"

namespace mini::crypto {

using hmac_sha256 = MINI_CRYPTO_HMAC_NAMESPACE::hmac<MINI_CRYPTO_HMAC_NAMESPACE::hash<hash_algorithm_type::sha256>>;

}
