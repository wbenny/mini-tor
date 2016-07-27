#pragma once
#include <mini/ptr.h>
#include <mini/byte_buffer.h>

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto {

class sha1
{
  public:
    MINI_MAKE_NONCOPYABLE(sha1);

    ~sha1(
      void
      );

    void
    init(
      void
      );

    void
    destroy(
      void
      );

    void
    update(
      const byte_buffer_ref input
      );

    void
    get(
      uint8_t output[20]
      );

    byte_buffer
    get(
      void
      );

    ptr<sha1>
    duplicate(
      void
      ) const;

    static byte_buffer
    hash(
      const byte_buffer_ref input
      );

  private:
    friend class provider;

    sha1(
      provider* crypto_provider
      );

    sha1(
      provider* crypto_provider,
      const sha1& other
      );

    void
    duplicate_internal(
      const sha1& other
      );

    provider* _provider;
    HCRYPTHASH _hash;
};

}

