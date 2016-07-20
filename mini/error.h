#pragma once
#include <cstdint>

namespace mini {

enum class error_code : uint32_t
{
  no_error,
  general_error,
  socket_error,
};

}
