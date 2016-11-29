#pragma once
#include <mini/string.h>
#include <mini/io/stream.h>

namespace mini::net::http::client {

string
get(
  const string_ref host,
  uint16_t port,
  const string_ref path
  );

string
get(
  const string_ref host,
  uint16_t port,
  const string_ref path,
  io::stream& sock
  );

}
