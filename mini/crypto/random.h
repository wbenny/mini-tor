#pragma once
#include <windows.h>
#include <wincrypt.h>
#include <cstdint>

#include "provider.h"
#include <mini/common.h>
#include <mini/byte_buffer.h>

namespace mini::crypto {

class random
{
  public:
    MINI_MAKE_NONCOPYABLE(random);

    template <typename T>
    T
    get_random()
    {
      T result;
      CryptGenRandom(
        _provider->get_handle(),
        sizeof(T),
        (BYTE*)&result);

      return result;
    }

    template <typename T>
    T
    get_random(
        T max
      )
    {
      return get_random() % max;
    }

    byte_buffer
    get_random_bytes(
      size_t byte_count
      )
    {
      byte_buffer result(byte_count);
      get_random_bytes(result);

      return result;
    }

    void
    get_random_bytes(
      mutable_byte_buffer_ref output
      )
    {
      CryptGenRandom(
        _provider->get_handle(),
        (DWORD)output.get_size(),
        output.get_buffer());
    }

  private:
    friend class provider;

    random(
      provider* crypto_provider
      )
      : _provider(crypto_provider)
    {

    }

    provider* _provider;
};

}
