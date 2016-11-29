#pragma once
#include "hash_template_base.h"

namespace mini::crypto {

using sha1 = hash_template_base<CALG_SHA1, 160>;

}
