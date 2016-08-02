#pragma once
#include <mini/byte_buffer.h>

#include <windows.h>
#include <wincrypt.h>

//
// http://stackoverflow.com/questions/1231178/load-an-x509-pem-file-into-windows-cryptoapi
// https://svn.apache.org/repos/asf/santuario/xml-security-cpp/trunk/xsec/enc/WinCAPI/WinCAPICryptoKeyRSA.cpp
//

namespace mini::crypto {

class rsa
{
  public:
    MINI_MAKE_NONCOPYABLE(rsa);

    ~rsa(
      void
      );

    void
    set_public_key(
      const byte_buffer_ref public_key
      );

    void
    set_private_key(
      const byte_buffer_ref private_key
      );

    byte_buffer
    public_encrypt(
      const byte_buffer_ref input,
      bool do_final
      );

    byte_buffer
    private_decrypt(
      const byte_buffer_ref input,
      bool do_final
      );

    static byte_buffer
    public_encrypt(
      const byte_buffer_ref input,
      const byte_buffer_ref public_key
      );

    static byte_buffer
    private_decrypt(
      const byte_buffer_ref input,
      const byte_buffer_ref public_key
      );

  private:
    friend class provider;

    rsa(
      provider* crypto_provider
      );

    provider* _provider;

    void
    import_key(
      const byte_buffer_ref key_blob
      );

    HCRYPTKEY _key = 0;
    DWORD _key_size = 0;

    byte_buffer _key_blob;
};

}
