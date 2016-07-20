#pragma once
#include <mini/string.h>

#include <cstdint>

namespace mini::net::http::client {

string
get(
  const string_ref host,
  uint16_t port,
  const string_ref path
  );

}
