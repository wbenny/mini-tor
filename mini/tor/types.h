#pragma once
#include <mini/collections/pair_list.h>
#include <mini/string.h>

#include "onion_router.h"

#include <cstdint>

namespace mini::tor {

using circuit_id_type       = uint32_t;
using circuit_id_v3_type    = uint16_t;
using tor_stream_id_type    = uint16_t;
using payload_size_type     = uint16_t;
using protocol_version_type = uint16_t;

using onion_router_map      = collections::pair_list<string, onion_router*>;

enum class circuit_node_type
{
  normal,
  introduction_point,
};

}
