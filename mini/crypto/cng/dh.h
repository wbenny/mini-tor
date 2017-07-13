#pragma once
#include "key.h"
#include "../common.h"

#include <mini/byte_buffer.h>

#include <windows.h>
#include <bcrypt.h>

namespace mini::crypto::cng {

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
        using key_type = provider_key_asymmetric_tag;
        static constexpr auto blob_type = BCRYPT_DH_PUBLIC_BLOB;
        static constexpr auto get_handle = &provider::get_dh_handle;
      };

      BCRYPT_DH_KEY_BLOB header;
      BYTE               prime[key_size_in_bytes];     // modulus
      BYTE               generator[key_size_in_bytes]; // generator
      BYTE               y[key_size_in_bytes];         // public
    };

  private:
    blob _blob;

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
        using key_type = provider_key_asymmetric_tag;
        static constexpr auto blob_type = BCRYPT_DH_PRIVATE_BLOB;
        static constexpr auto get_handle = &provider::get_dh_handle;
      };

      BCRYPT_DH_KEY_BLOB header;
      BYTE               prime[key_size_in_bytes];     // modulus
      BYTE               generator[key_size_in_bytes]; // generator
      BYTE               y[key_size_in_bytes];         // public
      BYTE               secret[key_size_in_bytes];    // exponent
    };

  private:
    blob _blob;
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
