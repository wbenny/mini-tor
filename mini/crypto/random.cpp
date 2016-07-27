#pragma once
#include "random.h"

namespace mini::crypto {

random::random(
  provider* crypto_provider
  )
  : _provider(crypto_provider)
{

}

byte_buffer
random::get_random_bytes(
  size_t byte_count
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
    _provider->get_handle(),
    (DWORD)output.get_size(),
    output.get_buffer());
}

ptr<random> random_device = provider_factory.create_random();

}
