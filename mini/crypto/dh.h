#pragma once
#include <mini/byte_buffer.h>

#include <windows.h>
#include <wincrypt.h>

#include "key.h"

//
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa381969(v=vs.85).aspx
// https://social.msdn.microsoft.com/Forums/sqlserver/en-US/a68aeb09-83ab-4d67-8e22-f0a99e1b4f9a/how-to-export-diffiehellman-shared-key?forum=windowssecurity
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa381970(v=vs.85).aspx
//

namespace mini::crypto {

#define DH_PUBLIC_MAGIC  0x31484400
#define DH_PRIVATE_MAGIC 0x32484400

template <size_type KEY_SIZE> class dh_public_key;
template <size_type KEY_SIZE> class dh_private_key;

template <
  size_type KEY_SIZE
>
class dh_public_key
  : public key
{
  MINI_MAKE_NONCOPYABLE(dh_public_key);

  public:
    static constexpr size_type key_size          = KEY_SIZE;
    static constexpr size_type key_size_in_bytes = KEY_SIZE / 8;

    struct blob
    {
      using provider_type = provider_dh_tag;

      PUBLICKEYSTRUC  publickeystruc;
      DHPUBKEY        dhpubkey;
      BYTE            y[key_size_in_bytes];
    };

    dh_public_key(
      void
      );

    dh_public_key(
      const byte_buffer_ref y,
      little_endian_tag
      );

    dh_public_key(
      const byte_buffer_ref y,
      big_endian_tag
      );

    dh_public_key(
      const byte_buffer_ref y
      );

    dh_public_key(
      dh_public_key&& other
      );

    dh_public_key&
    operator=(
      dh_public_key&& other
      );

    void
    swap(
      dh_public_key& other
      );

    //
    // import.
    //

    void
    import(
      const byte_buffer_ref y
      );

    void
    import(
      const byte_buffer_ref y,
      little_endian_tag
      );

    void
    import(
      const byte_buffer_ref y,
      big_endian_tag
      );

    //
    // getters.
    //

    byte_buffer
    get_y(
      void
      ) const;

  private:
    //
    // big endian.
    //

    byte_buffer _y;
};

template <
  size_type KEY_SIZE
>
class dh_private_key
  : public key
{
  MINI_MAKE_NONCOPYABLE(dh_private_key);

  public:
    static constexpr size_type key_size          = KEY_SIZE;
    static constexpr size_type key_size_in_bytes = KEY_SIZE / 8;

    struct blob
    {
      using provider_type = provider_dh_tag;

      PUBLICKEYSTRUC  publickeystruc;
      DHPUBKEY        dhpubkey;
      BYTE            prime[key_size_in_bytes];
      BYTE            generator[key_size_in_bytes];
      BYTE            secret[key_size_in_bytes];
    };

    dh_private_key(
      void
      );

    dh_private_key(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus,
      const byte_buffer_ref exponent
      );

    dh_private_key(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus,
      const byte_buffer_ref exponent,
      little_endian_tag
      );

    dh_private_key(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus,
      const byte_buffer_ref exponent,
      big_endian_tag
      );

    dh_private_key(
      dh_private_key&& other
      );

    dh_private_key&
    operator=(
      dh_private_key&& other
      );

    void
    swap(
      dh_private_key& other
      );

    //
    // import.
    //

    void
    import(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus,
      const byte_buffer_ref exponent
      );

    void
    import(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus,
      const byte_buffer_ref exponent,
      little_endian_tag
      );

    void
    import(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus,
      const byte_buffer_ref exponent,
      big_endian_tag
      );

    //
    // export.
    //

    dh_public_key<KEY_SIZE>
    export_public_key(
      void
      ) const;

    //
    // getters.
    //

    byte_buffer
    get_generator(
      void
      ) const;

    byte_buffer
    get_modulus(
      void
      ) const;

    byte_buffer
    get_exponent(
      void
      ) const;

    byte_buffer
    get_shared_secret(
      const dh_public_key<KEY_SIZE>& other_public_key
      ) const;

  private:
    void
    import_impl(
      void
      );

    //
    // big endian.
    //

    byte_buffer _generator;
    byte_buffer _modulus;  // aka prime
    byte_buffer _exponent; // aka private_key
};

template <
  size_type KEY_SIZE
>
class dh
{
  public:
    static constexpr size_type key_size          = KEY_SIZE;
    static constexpr size_type key_size_in_bytes = KEY_SIZE / 8;

    using public_key  = dh_public_key<key_size>;
    using private_key = dh_private_key<key_size>;

    static private_key
    generate_private_key(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus
      );
};

}

#include "dh.inl"
