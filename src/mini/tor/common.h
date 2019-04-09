#pragma once
#include <mini/string.h>
#include <mini/collections/pair_list.h>

#include "onion_router.h"

namespace mini::tor {

using circuit_id_type       = uint32_t;
using circuit_id_v3_type    = uint16_t;
using tor_stream_id_type    = uint16_t;
using payload_size_type     = uint16_t;
using protocol_version_type = uint16_t;

enum class circuit_node_type
{
  normal,
  introduction_point,
};

enum class handshake_type
{
  tap,
  ntor
};

static constexpr handshake_type preferred_handshake_type = handshake_type::ntor;

}
