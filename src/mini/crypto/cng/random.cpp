#include "random.h"
#include "provider.h"

namespace mini::crypto::cng {

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
  BCryptGenRandom(
    provider_factory.get_rng_handle(),
    output.get_buffer(),
    static_cast<ULONG>(output.get_size()),
    0);
}

random random_device;

}
