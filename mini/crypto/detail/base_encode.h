#pragma once
#include <mini/string.h>
#include <mini/byte_buffer.h>

#include <windows.h>

namespace mini::crypto::detail {

void
base_encode_impl(
  DWORD flags,
  const byte_type* input,
  size_type input_size,
  char* output,
  size_type& output_size
  );

void
base_decode_impl(
  DWORD flags,
  const char* input,
  size_type input_size,
  byte_type* output,
  size_type& output_size
  );

}
