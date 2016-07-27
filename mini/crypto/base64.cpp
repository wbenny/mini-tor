#pragma once
#include "base64.h"

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto::base64 {

void
encode_impl(
  const uint8_t* input,
  size_t input_size,
  char*& output,
  size_t& output_size,
  bool get_only_size,
  bool alloc_buffer
  )
{
  if (get_only_size)
  {
    output_size = 0;
    CryptBinaryToString(
      input,
      (DWORD)input_size,
      CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
      NULL,
      (DWORD*)&output_size);
  }
  else
  {
    if (alloc_buffer)
    {
      output = new char[output_size];
    }

    CryptBinaryToString(
      input,
      (DWORD)input_size,
      CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
      output,
      (DWORD*)&output_size);
  }
}

void
decode_impl(
  const char* input,
  size_t input_size,
  uint8_t*& output,
  size_t& output_size,
  bool get_only_size,
  bool alloc_buffer
  )
{
  //
  // CRYPT_STRING_BASE64_ANY:
  //
  // Tries the following, in order:
  //   CRYPT_STRING_BASE64HEADER
  //   CRYPT_STRING_BASE64
  //

  if (get_only_size)
  {
    output_size = 0;
    CryptStringToBinary(
      input,
      (DWORD)input_size,
      CRYPT_STRING_BASE64_ANY,
      NULL,
      (DWORD*)&output_size,
      NULL,
      NULL);
  }
  else
  {
    if (alloc_buffer)
    {
      output = new uint8_t[output_size];
    }

    CryptStringToBinary(
      input,
      (DWORD)input_size,
      CRYPT_STRING_BASE64_ANY,
      output,
      (DWORD*)&output_size,
      NULL,
      NULL);
  }
}

void
encode(
  const uint8_t* input,
  size_t input_size,
  char*& output,
  size_t& output_size)
{
  encode_impl(input, input_size, output, output_size, true, true);
  encode_impl(input, input_size, output, output_size, false, true);
}

void
decode(
  const char* input,
  size_t input_size,
  uint8_t*& output,
  size_t& output_size)
{
  decode_impl(input, input_size, output, output_size, true, true);
  decode_impl(input, input_size, output, output_size, false, true);
}

void
encode(
  const uint8_t* input,
  size_t input_size,
  char*& output)
{
  size_t output_size;
  encode(input, input_size, output, output_size);
}

void
decode(
  const char* input,
  size_t input_size,
  uint8_t*& output)
{
  size_t output_size;
  decode(input, input_size, output, output_size);
}

string
encode(
  const byte_buffer_ref input
  )
{
  string output;
  size_t output_size;
  char* output_buffer;
  encode_impl(input.get_buffer(), input.get_size(), output_buffer, output_size, true, false);

  output.resize(output_size - 1);
  output_buffer = output.get_buffer();
  encode_impl(input.get_buffer(), input.get_size(), output_buffer, output_size, false, false);

  return output;
}

byte_buffer
decode(
  const string_ref input
  )
{
  byte_buffer output;
  size_t output_size;
  uint8_t* output_buffer;
  decode_impl(input.get_buffer(), input.get_size(), output_buffer, output_size, true, false);

  output.resize(output_size);
  output_buffer = output.get_buffer();
  decode_impl(input.get_buffer(), input.get_size(), output_buffer, output_size, false, false);

  return output;
}

}
