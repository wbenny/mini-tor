#pragma once
#include <mini/string.h>
#include <mini/byte_buffer.h>

namespace mini::crypto::capi {

class base16
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
