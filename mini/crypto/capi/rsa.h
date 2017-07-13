#pragma once
#include "key.h"
#include "../common.h"

#include <mini/byte_buffer.h>
#include <mini/string.h>

#include <windows.h>
#include <wincrypt.h>

//
// http://stackoverflow.com/questions/1231178/load-an-x509-pem-file-into-windows-cryptoapi
// https://svn.apache.org/repos/asf/santuario/xml-security-cpp/trunk/xsec/enc/WinCAPI/WinCAPICryptoKeyRSA.cpp
//

namespace mini::crypto::capi {

template <size_type KEY_SIZE> class rsa_public_key;
template <size_type KEY_SIZE> class rsa_private_key;

template <
  size_type KEY_SIZE
>
class rsa_public_key
  : public key
{
  MINI_MAKE_NONCOPYABLE(rsa_public_key);

  public:
    static constexpr size_type key_size          = KEY_SIZE;
    static constexpr size_type key_size_in_bytes = KEY_SIZE / 8;

    rsa_public_key(
      void
      ) = default;

    rsa_public_key(
      rsa_public_key&& other
      );

    rsa_public_key&
    operator=(
      rsa_public_key&& other
      );

    void
    swap(
      rsa_public_key& other
      );

    //
    // import.
    //

    static rsa_public_key<KEY_SIZE>
    make_from_der(
      const byte_buffer_ref key
      );

    static rsa_public_key<KEY_SIZE>
    make_from_pem(
      const string_ref key
      );

    void
    import_from_der(
      const byte_buffer_ref key
      );

    void
    import_from_pem(
      const string_ref key
      );

    byte_buffer
    encrypt(
      const byte_buffer_ref input,
      rsa_encryption_padding padding,
      bool do_final
      );

  public:
    struct blob
    {
      struct provider_type
      {
        static constexpr auto get_handle = &provider::get_rsa_aes_handle;
      };

      PUBLICKEYSTRUC  publickeystruc;
      RSAPUBKEY       rsapubkey;
      BYTE            modulus[key_size_in_bytes];
    };

  private:
    blob _blob;

    friend class rsa_private_key<KEY_SIZE>;
};

template <
  size_type KEY_SIZE
>
class rsa_private_key
  : public key
{
  MINI_MAKE_NONCOPYABLE(rsa_private_key);

  public:
    static constexpr size_type key_size          = KEY_SIZE;
    static constexpr size_type key_size_in_bytes = KEY_SIZE / 8;

    rsa_private_key(
      void
      ) = default;

    rsa_private_key(
      rsa_private_key&& other
      );

    rsa_private_key&
    operator=(
      rsa_private_key&& other
      );

    void
    swap(
      rsa_private_key& other
      );

    //
    // import.
    //

    static rsa_private_key<KEY_SIZE>
    make_from_der(
      const byte_buffer_ref key
      );

    static rsa_private_key<KEY_SIZE>
    make_from_pem(
      const string_ref key
      );

    void
    import_from_der(
      const byte_buffer_ref key
      );

    void
    import_from_pem(
      const string_ref key
      );

    rsa_public_key<KEY_SIZE>
    export_public_key(
      void
      ) const;

    byte_buffer
    decrypt(
      const byte_buffer_ref input,
      rsa_encryption_padding padding,
      bool do_final
      );

  public:
    struct blob
    {
      struct provider_type
      {
        static constexpr auto get_handle = &provider::get_rsa_aes_handle;
      };

      PUBLICKEYSTRUC  publickeystruc;
      RSAPUBKEY       rsapubkey;
      BYTE            modulus[key_size_in_bytes];

      //
      // fields of private key.
      //

      BYTE            prime1[key_size_in_bytes / 2];
      BYTE            prime2[key_size_in_bytes / 2];
      BYTE            exponent1[key_size_in_bytes / 2];
      BYTE            exponent2[key_size_in_bytes / 2];
      BYTE            coefficient[key_size_in_bytes / 2];
      BYTE            privateExponent[key_size_in_bytes];
    };

  private:
    blob _blob;
};

template <
  size_type KEY_SIZE
>
class rsa
{
  MINI_MAKE_NONCOPYABLE(rsa);

  public:
    static constexpr size_type key_size          = KEY_SIZE;
    static constexpr size_type key_size_in_bytes = KEY_SIZE / 8;

    using public_key  = rsa_public_key<KEY_SIZE>;
    using private_key = rsa_private_key<KEY_SIZE>;
};

}

#include "rsa.inl"
