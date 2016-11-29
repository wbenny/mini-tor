#pragma once
#include <mini/byte_buffer.h>
#include <mini/string.h>

#include <windows.h>
#include <wincrypt.h>

#include "key.h"

//
// http://stackoverflow.com/questions/1231178/load-an-x509-pem-file-into-windows-cryptoapi
// https://svn.apache.org/repos/asf/santuario/xml-security-cpp/trunk/xsec/enc/WinCAPI/WinCAPICryptoKeyRSA.cpp
//

namespace mini::crypto {

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

    struct blob
    {
      using provider_type = provider_rsa_aes_tag;

      PUBLICKEYSTRUC  publickeystruc;
      RSAPUBKEY       rsapubkey;
      BYTE            modulus[key_size_in_bytes];
    };

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

    struct blob
    {
      using provider_type = provider_rsa_aes_tag;

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

    using public_key  = rsa_public_key<key_size>;
    using private_key = rsa_private_key<key_size>;

    enum class encryption_padding : DWORD
    {
      pkcs1     = 0,
      oaep_sha1 = CRYPT_OAEP
    };

    rsa(
      void
      );

    rsa(
      public_key&& key
      );

    rsa(
      private_key&& key
      );

    ~rsa(
      void
      );

    void
    set_public_key(
      public_key&& key
      );

    void
    set_private_key(
      private_key&& key
      );

    byte_buffer
    public_encrypt(
      const byte_buffer_ref input,
      encryption_padding padding,
      bool do_final
      );

    byte_buffer
    private_decrypt(
      const byte_buffer_ref input,
      encryption_padding padding,
      bool do_final
      );

    static byte_buffer
    public_encrypt(
      public_key&& key,
      const byte_buffer_ref input,
      encryption_padding padding
      );

    static byte_buffer
    private_decrypt(
      private_key&& key,
      const byte_buffer_ref input,
      encryption_padding padding
      );

  private:
    void
    destroy_key(
      void
      );

    union
    {
      //
      // to access destroy() method.
      //
      key _key;

      public_key  _public_key;
      private_key _private_key;
    };
};

}

#include "rsa.inl"
