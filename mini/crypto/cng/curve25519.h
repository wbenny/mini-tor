#pragma once
#include "key.h"

#include <mini/byte_buffer.h>
#include <mini/string.h>

#include <windows.h>
#include <bcrypt.h>

namespace mini::crypto::cng {

//
// cng::curve25519 is available only on Win10+
// ref: https://msdn.microsoft.com/en-us/library/windows/desktop/mt632245(v=vs.85).aspx
//

class curve25519_public_key;
class curve25519_private_key;

class curve25519_public_key
  : public key
{
  MINI_MAKE_NONCOPYABLE(curve25519_public_key);

  public:
    static constexpr size_type key_size          = 255;
    static constexpr size_type key_size_in_bytes = 32;

    curve25519_public_key(
      void
      ) = default;

    curve25519_public_key(
      const byte_buffer_ref key
      );

    curve25519_public_key(
      curve25519_public_key&& other
      );

    curve25519_public_key&
    operator=(
      curve25519_public_key&& other
      );

    void
    swap(
      curve25519_public_key& other
      );

    //
    // import.
    //

    void
    import(
      const byte_buffer_ref key
      );

    //
    // getters.
    //

    byte_buffer
    get_public_key_buffer(
      void
      ) const;

  public:
    struct blob
    {
      struct provider_type
      {
        using key_type = provider_key_asymmetric_tag;
        static constexpr auto blob_type = BCRYPT_ECCPUBLIC_BLOB;
        static constexpr auto get_handle = &provider::get_curve25519_handle;
      };

      BCRYPT_ECCKEY_BLOB ecckeyblob;
      BYTE               X[key_size_in_bytes];
      BYTE               Y[key_size_in_bytes];
    };

  private:
    blob _blob;

    friend class curve25519_private_key;
};

class curve25519_private_key
  : public key
{
  MINI_MAKE_NONCOPYABLE(curve25519_private_key);

  public:
    static constexpr size_type key_size          = 255;
    static constexpr size_type key_size_in_bytes = 32;

    curve25519_private_key(
      void
      ) = default;

    curve25519_private_key(
      const byte_buffer_ref key
      );

    curve25519_private_key(
      curve25519_private_key&& other
      );

    curve25519_private_key&
    operator=(
      curve25519_private_key&& other
      );

    void
    swap(
      curve25519_private_key& other
      );

    //
    // import.
    //

    static curve25519_private_key
    generate(
      void
      );

    void
    import(
      const byte_buffer_ref key
      );

    //
    // export.
    //

    curve25519_public_key
    export_public_key(
      void
      ) const;

    //
    // getters.
    //

    byte_buffer_ref
    get_public_key_buffer(
      void
      ) const;

    byte_buffer_ref
    get_private_key_buffer(
      void
      ) const;

    byte_buffer
    get_shared_secret(
      const curve25519_public_key& other_public_key
      ) const;

  public:
    struct blob
    {
      struct provider_type
      {
        using key_type = provider_key_asymmetric_tag;
        static constexpr auto blob_type = BCRYPT_ECCPRIVATE_BLOB;
        static constexpr auto get_handle = &provider::get_curve25519_handle;
      };

      BCRYPT_ECCKEY_BLOB ecckeyblob;
      BYTE               X[key_size_in_bytes];
      BYTE               Y[key_size_in_bytes];
      BYTE               d[key_size_in_bytes];
    };

  private:
    blob _blob;
};

class curve25519
{
  MINI_MAKE_NONCONSTRUCTIBLE(curve25519);

  public:
    static constexpr size_type key_size          = 255;
    static constexpr size_type key_size_in_bytes = 32;

    using public_key  = curve25519_public_key;
    using private_key = curve25519_private_key;
};

}

