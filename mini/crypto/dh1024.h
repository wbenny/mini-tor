#pragma once
#include <mini/byte_buffer.h>

#include <windows.h>
#include <wincrypt.h>

//
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa381969(v=vs.85).aspx
// https://social.msdn.microsoft.com/Forums/sqlserver/en-US/a68aeb09-83ab-4d67-8e22-f0a99e1b4f9a/how-to-export-diffiehellman-shared-key?forum=windowssecurity
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa381970(v=vs.85).aspx
//

namespace mini::crypto {

class dh1024
{
  public:
    MINI_MAKE_NONCOPYABLE(dh1024);

    static constexpr size_t key_size = 128;

    ~dh1024(
      void
      );

    void
    generate_key(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus
      );

    void
    import_key(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus,
      const byte_buffer_ref exponent
      );

    byte_buffer
    get_public_key(
      void
      );

    byte_buffer
    get_private_key(
      void
      );

    byte_buffer
    get_shared_secret(
      const byte_buffer_ref other_public_key
      );

  private:
    friend class provider;

    dh1024(
      provider* crypto_provider
      );

    provider* _provider;

    byte_buffer _generator;
    byte_buffer _modulus;

    byte_buffer _public_key;  // (generator^private_key) % modulus
    byte_buffer _private_key;
};

}

