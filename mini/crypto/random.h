#pragma once
#include "provider.h"

#include <mini/byte_buffer.h>

#include <windows.h>
#include <wincrypt.h>

#include <type_traits>

namespace mini::crypto {

class random
{
  public:
    MINI_MAKE_NONCOPYABLE(random);

    byte_buffer
    get_random_bytes(
      size_t byte_count
      );

    void
    get_random_bytes(
      mutable_byte_buffer_ref output
      );

    template <
      typename T,
      typename = std::enable_if_t<std::is_integral_v<T>>
    >
    T
    get_random(
      void
      )
    {
      T result;
      get_random_bytes(mutable_byte_buffer_ref(
        reinterpret_cast<byte_type*>(&result),
        reinterpret_cast<byte_type*>(&result) + sizeof(T)));

      return result;
    }

    template <
      typename T,
      typename = std::enable_if_t<std::is_integral_v<T>>
    >
    T
    get_random(
        T max
      )
    {
      return get_random<std::make_unsigned_t<T>>() % max;
    }

  private:
    friend class provider;

    random(
      provider* crypto_provider
      );

    provider* _provider;
};

extern ptr<random> random_device;

}
