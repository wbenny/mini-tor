#pragma once
#include "key.h"
#include "../common.h"

#include <mini/byte_buffer.h>

#include <windows.h>
#include <wincrypt.h>

//
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa381969(v=vs.85).aspx
// https://social.msdn.microsoft.com/Forums/sqlserver/en-US/a68aeb09-83ab-4d67-8e22-f0a99e1b4f9a/how-to-export-diffiehellman-shared-key?forum=windowssecurity
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa381970(v=vs.85).aspx
//

namespace mini::crypto::capi {

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

    dh_public_key(
      void
      );

    dh_public_key(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus,
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
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus,
      const byte_buffer_ref y
      );

    //
    // getters.
    //

    byte_buffer_ref
    get_generator(
      void
      ) const;

    byte_buffer_ref
    get_modulus(
      void
      ) const;

    byte_buffer_ref
    get_y(
      void
      ) const;

  public:
    struct blob
    {
      struct provider_type
      {
        static constexpr auto get_handle = &provider::get_dh_handle;
      };

      PUBLICKEYSTRUC  publickeystruc;
      DHPUBKEY        dhpubkey;
      BYTE            y[key_size_in_bytes];
    };

  private:
    //
    // values in the blob are in big endian for faster caching.
    //
    // it is important to convert them to little endian before import
    // and after export because of how MS CryptoAPI works.
    //
    blob _blob;

    //
    // unfortunatelly, these are not part of the CryptoAPI's
    // DH public key blob.
    //
    byte_type _generator[key_size_in_bytes];
    byte_type _prime[key_size_in_bytes];

    friend class dh_private_key<key_size>;
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

    dh_private_key(
      void
      );

    dh_private_key(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus,
      const byte_buffer_ref exponent
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

    static dh_private_key
    generate(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus
      );

    void
    import(
      const byte_buffer_ref generator,
      const byte_buffer_ref modulus,
      const byte_buffer_ref exponent
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

    byte_buffer_ref
    get_generator(
      void
      ) const;

    byte_buffer_ref
    get_modulus(
      void
      ) const;

    byte_buffer_ref
    get_exponent(
      void
      ) const;

    byte_buffer_ref
    get_y(
      void
      ) const;

    byte_buffer
    get_shared_secret(
      const dh_public_key<KEY_SIZE>& other_public_key
      ) const;

    byte_buffer
    get_shared_secret(
      const byte_buffer_ref other_public_key_y
      ) const;

  public:
    struct blob
    {
      struct provider_type
      {
        static constexpr auto get_handle = &provider::get_dh_handle;
      };

      PUBLICKEYSTRUC  publickeystruc;
      DHPUBKEY        dhpubkey;
      BYTE            prime[key_size_in_bytes];     // modulus
      BYTE            generator[key_size_in_bytes]; // generator
      BYTE            secret[key_size_in_bytes];    // exponent
    };

  private:
    void
    import_impl(
      void
      );

    //
    // values in the blob are in big endian for faster caching.
    //
    // it is important to convert them to little endian before import
    // and after export because of how MS CryptoAPI works.
    //
    blob _blob;
    dh_public_key<KEY_SIZE> _public_key;
};

template <
  size_type KEY_SIZE
>
class dh
{
  MINI_MAKE_NONCONSTRUCTIBLE(dh);

  public:
    static constexpr size_type key_size          = KEY_SIZE;
    static constexpr size_type key_size_in_bytes = KEY_SIZE / 8;

    using public_key  = dh_public_key<KEY_SIZE>;
    using private_key = dh_private_key<KEY_SIZE>;
};

}

#include "dh.inl"
