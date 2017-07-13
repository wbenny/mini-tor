#include "base64.h"
#include "detail/base_encode.h"

#include <wincrypt.h>

namespace mini::crypto::capi {

static void
encode_impl(
  const byte_type* input,
  size_type input_size,
  char* output,
  size_type& output_size
  )
{
  detail::base_encode_impl(
    CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
    input,
    input_size,
    output,
    output_size);
}

static void
decode_impl(
  const char* input,
  size_type input_size,
  byte_type* output,
  size_type& output_size
  )
{
  //
  // CRYPT_STRING_BASE64_ANY:
  //
  // Tries the following, in order:
  //   CRYPT_STRING_BASE64HEADER
  //   CRYPT_STRING_BASE64
  //

  detail::base_decode_impl(
    CRYPT_STRING_BASE64_ANY,
    input,
    input_size,
    output,
    output_size);
}

string
base64::encode(
  const byte_buffer_ref input
  )
{
  string output;
  size_type output_size;

  encode_impl(input.get_buffer(), input.get_size(), nullptr, output_size);

  output.resize(output_size - 1);
  encode_impl(input.get_buffer(), input.get_size(), output.get_buffer(), output_size);

  return output;
}

byte_buffer
base64::decode(
  const string_ref input
  )
{
  byte_buffer output;
  size_type output_size;

  decode_impl(input.get_buffer(), input.get_size(), nullptr, output_size);

  output.resize(output_size);
  decode_impl(input.get_buffer(), input.get_size(), output.get_buffer(), output_size);

  return output;
}

}
