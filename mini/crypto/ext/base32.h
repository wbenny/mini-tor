#pragma once
#include <mini/string.h>
#include <mini/byte_buffer.h>

namespace mini::crypto::ext {

class base32
{
  public:
    static string
    encode(
      const byte_buffer_ref input
      );

    static byte_buffer
    decode(
      const string_ref input
      );
};

}
