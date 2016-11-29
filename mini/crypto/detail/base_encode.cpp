#include "base_encode.h"

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto::detail {

void
base_encode_impl(
  DWORD flags,
  const byte_type* input,
  size_type input_size,
  char* output,
  size_type& output_size
  )
{
  if (!output)
  {
    output_size = 0;
  }

  CryptBinaryToString(
    input,
    static_cast<DWORD>(input_size),
    flags,
    output,
    reinterpret_cast<DWORD*>(&output_size));
}

void
base_decode_impl(
  DWORD flags,
  const char* input,
  size_type input_size,
  byte_type* output,
  size_type& output_size
  )
{
  if (!output)
  {
    output_size = 0;
  }

  CryptStringToBinary(
    input,
    static_cast<DWORD>(input_size),
    flags,
    output,
    reinterpret_cast<DWORD*>(&output_size),
    NULL,
    NULL);
}

}
