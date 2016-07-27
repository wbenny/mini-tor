#include "rsa.h"
#include "provider.h"

namespace mini::crypto {

rsa::rsa(
  provider* crypto_provider
  )
  : _provider(crypto_provider)
{

}

rsa::~rsa(
  void
  )
{
  CryptDestroyKey(_key);
}

void
rsa::set_public_key(
  const byte_buffer_ref public_key
  )
{
  BOOL result;

  DWORD key_blob_size;
  result = CryptDecodeObject(
    X509_ASN_ENCODING,
    RSA_CSP_PUBLICKEYBLOB,
    public_key.get_buffer(),
    (DWORD)public_key.get_size(),
    0,
    nullptr,
    &key_blob_size);

  _key_blob.resize(key_blob_size);
  result = CryptDecodeObject(
    X509_ASN_ENCODING,
    RSA_CSP_PUBLICKEYBLOB,
    public_key.get_buffer(),
    (DWORD)public_key.get_size(),
    0,
    _key_blob.get_buffer(),
    &key_blob_size);

  import_key(_key_blob);
}

void
rsa::set_private_key(
  const byte_buffer_ref private_key
  )
{
  BOOL result;

  DWORD key_blob_size;
  result = CryptDecodeObject(
    X509_ASN_ENCODING,
    PKCS_RSA_PRIVATE_KEY,
    private_key.get_buffer(),
    (DWORD)private_key.get_size(),
    0,
    nullptr,
    &key_blob_size);

  _key_blob.resize(key_blob_size);

  result = CryptDecodeObject(
    X509_ASN_ENCODING,
    PKCS_RSA_PRIVATE_KEY,
    private_key.get_buffer(),
    (DWORD)private_key.get_size(),
    0,
    _key_blob.get_buffer(),
    &key_blob_size);

  import_key(_key_blob);
}

byte_buffer
rsa::public_encrypt(
  const byte_buffer_ref input,
  bool do_final
  )
{
  byte_buffer output;
  output.resize(_key_size);

  memory::copy(output.get_buffer(), input.get_buffer(), input.get_size());

  DWORD dword_input_size = (DWORD)input.get_size();
  BOOL result;
  result = CryptEncrypt(
    _key,
    0,
    do_final,
    CRYPT_OAEP,
    output.get_buffer(),
    &dword_input_size,
    _key_size);

  for (DWORD i = 0; i < (_key_size / 2); i++)
  {
    BYTE c = output[i];
    output[i] = output[_key_size - 1 - i];
    output[_key_size - 1 - i] = c;
  }

  output.resize(dword_input_size); // is this necessary?
  return output;
}

byte_buffer
rsa::private_decrypt(
  const byte_buffer_ref input,
  bool do_final
  )
{
  byte_buffer output;
  output.resize(_key_size);

  memory::copy(output.get_buffer(), input.get_buffer(), input.get_size());

  for (DWORD i = 0; i < (_key_size / 2); i++)
  {
    BYTE c = output[i];
    output[i] = output[_key_size - 1 - i];
    output[_key_size - 1 - i] = c;
  }

  DWORD dword_input_size = (DWORD)input.get_size();
  BOOL result;
  result = CryptDecrypt(
    _key,
    0,
    do_final,
    CRYPT_OAEP,
    output.get_buffer(),
    &dword_input_size);

  output.resize(dword_input_size); // is this necessary?
  return output;
}

void
rsa::import_key(
  const byte_buffer_ref key_blob
  )
{
  BOOL result;
  result = CryptImportKey(
    _provider->get_handle(),
    key_blob.get_buffer(),
    (DWORD)key_blob.get_size(),
    NULL,
    0,
    &_key);

  DWORD param_size = sizeof(_key_size);
  CryptGetKeyParam(
    _key,
    KP_KEYLEN,
    (BYTE*)&_key_size,
    &param_size,
    0);

  _key_size /= 8;
}

byte_buffer
rsa::public_encrypt(
  const byte_buffer_ref input,
  const byte_buffer_ref public_key
  )
{
  ptr<rsa> rsa = provider_factory.create_rsa();
  rsa->set_public_key(public_key);
  return rsa->public_encrypt(input, true);
}

byte_buffer
rsa::private_decrypt(
  const byte_buffer_ref input,
  const byte_buffer_ref private_key
  )
{
  ptr<rsa> rsa = provider_factory.create_rsa();
  rsa->set_private_key(private_key);
  return rsa->private_decrypt(input, true);
}

}
