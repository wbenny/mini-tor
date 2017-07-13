#pragma once
#include "common.h"
#include "capi/hash.h"
#include "cng/hash.h"

namespace mini::crypto {

using sha1 = MINI_CRYPTO_HASH_NAMESPACE::hash<hash_algorithm_type::sha1>;

}
