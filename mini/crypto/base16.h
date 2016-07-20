#pragma once
#include <cstdint>

#include <mini/string.h>
#include <mini/byte_buffer.h>

namespace mini::crypto::base16 {

string
encode(
  const byte_buffer_ref input
  );

byte_buffer
decode(
  const string_ref input
  );

}
