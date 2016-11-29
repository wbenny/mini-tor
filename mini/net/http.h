#pragma once
#include <mini/string.h>
#include <mini/io/stream.h>

namespace mini::net::http::client {

string
get_on_stream(
  io::stream& sock,
  const string_ref host,
  uint16_t port,
  const string_ref path
  );

string
get(
  const string_ref host,
  uint16_t port,
  const string_ref path
  );

}
