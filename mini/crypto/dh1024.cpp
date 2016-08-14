#include "dh1024.h"
#include "random.h"

namespace mini::crypto {

//
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa381970(v=vs.85).aspx#privkeydiffblob
//

#define DH_PUBLIC_MAGIC  0x31484400
#define DH_PRIVATE_MAGIC 0x32484400

struct DHPUB
{
  PUBLICKEYSTRUC  publickeystruc;
  DHPUBKEY        dhpubkey;
  BYTE            y[dh1024::key_size];
};

struct DHPRIV
{
  PUBLICKEYSTRUC  publickeystruc;
  DHPUBKEY        dhpubkey;
  BYTE            prime[dh1024::key_size];
  BYTE            generator[dh1024::key_size];
  BYTE            secret[dh1024::key_size];
};

dh1024::dh1024(
  provider* crypto_provider
  )
  : _provider(crypto_provider)
  , _public_key(key_size)
  , _private_key(key_size)
{

}

dh1024::~dh1024(
  void
  )
{

}

void
dh1024::generate_key(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus
  )
{
  auto rnd = provider_factory.create_random();

  //
  // generate private key.
  //
  byte_buffer private_key(key_size);
  rnd->get_random_bytes(private_key);

  import_key(generator, modulus, private_key);
}

void
dh1024::import_key(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus,
  const byte_buffer_ref exponent
  )
{
  mini_assert( generator.get_size() <= key_size );
  mini_assert(  exponent.get_size() <= key_size );
  mini_assert(   modulus.get_size() <= key_size );

  DHPRIV private_key_blob = {
    { PRIVATEKEYBLOB, CUR_BLOB_VERSION, 0, CALG_DH_EPHEM },
    { DH_PRIVATE_MAGIC, dh1024::key_size * 8 }
  };

  _generator   = generator;
  _modulus     = modulus;
  _private_key = exponent;

  //
  // big endian -> little endian
  //
  memory::reverse_copy(private_key_blob.prime, _modulus.get_buffer(), _modulus.get_size());
  memory::reverse_copy(private_key_blob.generator, _generator.get_buffer(), _generator.get_size());
  memory::reverse_copy(private_key_blob.secret, _private_key.get_buffer(), _private_key.get_size());

  HCRYPTKEY private_key_handle;
  BOOL result = CryptImportKey(
    _provider->get_dh_handle(),
    (BYTE*)&private_key_blob,
    sizeof(private_key_blob),
    NULL,
    CRYPT_EXPORTABLE,
    &private_key_handle);

  DHPUB public_key_blob;
  DWORD public_key_blob_size = sizeof(public_key_blob);
  result = CryptExportKey(
    private_key_handle,
    0,
    PUBLICKEYBLOB,
    0,
    (BYTE*)&public_key_blob,
    &public_key_blob_size);

  //
  // little endian -> big endian
  //
  memory::reverse_copy(&_public_key[0],  public_key_blob.y, key_size);

  CryptDestroyKey(private_key_handle);
}

byte_buffer
dh1024::get_public_key(
  void
  )
{
  return _public_key;
}

byte_buffer
dh1024::get_private_key(
  void
  )
{
  return _private_key;
}

byte_buffer
dh1024::get_shared_secret(
  const byte_buffer_ref other_public_key
  )
{
  //
  // yolo.
  //
  ptr<dh1024> helper = provider_factory.create_dh1024();
  helper->import_key(
    other_public_key,
    _modulus,
    _private_key
    );

  return helper->get_public_key();
}

}
