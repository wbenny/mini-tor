#include "random.h"
#include "provider.h"

namespace mini::crypto::capi {

byte_buffer
random::get_random_bytes(
  size_type byte_count
  )
{
  byte_buffer result(byte_count);
  get_random_bytes(result);

  return result;
}

void
random::get_random_bytes(
  mutable_byte_buffer_ref output
  )
{
  CryptGenRandom(
    provider_factory.get_rsa_aes_handle(),
    static_cast<DWORD>(output.get_size()),
    output.get_buffer());
}

random random_device;

}
