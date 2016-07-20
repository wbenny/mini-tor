#pragma once
#include <windows.h>
#include <wincrypt.h>
#include <cstdint>

#include "provider.h"
#include <mini/common.h>
#include <mini/byte_buffer.h>

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
    rsa(
      provider* crypto_provider
      );

    friend class provider;
    provider* _provider;

    void
    import_key(
      const byte_buffer_ref key_blob
      );

    //
    // struct public_key
    // {
    //   BLOBHEADER header;
    //   RSAPUBKEY key;
    //   uint8_t public_key_blob[1];
    // };
    //
    // struct private_key
    // {
    //   BLOBHEADER        blobheader;
    //   DSSPRIVKEY_VER3   dssprivkeyver3;
    //   BYTE p[dssprivkeyver3.bitlenP / 8]; // Where P is the prime modulus
    //   BYTE q[dssprivkeyver3.bitlenQ / 8]; // Where Q is a large factor of P-1
    //   BYTE g[dssprivkeyver3.bitlenP / 8]; // Where G is the generator parameter
    //   BYTE j[dssprivkeyver3.bitlenJ / 8]; // Where J is (P-1)/Q
    //   BYTE y[dssprivkeyver3.bitlenP / 8]; // Where Y is (G^X) mod P
    //   BYTE x[dssprivkeyver3.bitlenX / 8]; // Where X is the private exponent
    // };
    //

    HCRYPTKEY _key;
    DWORD _key_size;

    byte_buffer _key_blob;
};

}
