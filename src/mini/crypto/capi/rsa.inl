#include "rsa.h"
#include "base64.h"

namespace mini::crypto::capi {

namespace detail {

  //
  // map each value from rsa_encryption_padding to its
  // corresponding CryptoAPI definition.
  //
  static constexpr DWORD rsa_encryption_padding_map[] = {
    0,
    CRYPT_OAEP,
  };

}

//
// NOTE: there are not defined default constructors
// for rsa_public_key/rsa_private_key which
// prefill the _blob member.
// the reason is that the keys can be created only by importing
// which will already set the blob properties.
//

//
// RSA
// public key.
//

template <
  size_type KEY_SIZE
>
rsa_public_key<KEY_SIZE>::rsa_public_key(
  rsa_public_key&& other
  )
{
  swap(other);
}

template <
  size_type KEY_SIZE
>
rsa_public_key<KEY_SIZE>&
rsa_public_key<KEY_SIZE>::operator=(
  rsa_public_key&& other
  )
{
  swap(other);
  return *this;
}

template <
  size_type KEY_SIZE
>
void
rsa_public_key<KEY_SIZE>::swap(
  rsa_public_key& other
  )
{
  key::swap(other);
  mini::swap(_blob, other._blob);
}

//
// import.
//

template <
  size_type KEY_SIZE
>
rsa_public_key<KEY_SIZE>
rsa_public_key<KEY_SIZE>::make_from_der(
  const byte_buffer_ref key
  )
{
  rsa_public_key<KEY_SIZE> result;
  result.import_from_der(key);
  return result;
}

template <
  size_type KEY_SIZE
>
rsa_public_key<KEY_SIZE>
rsa_public_key<KEY_SIZE>::make_from_pem(
  const string_ref key
  )
{
  return make_from_der(base64::decode(key));
}

template <
  size_type KEY_SIZE
>
void
rsa_public_key<KEY_SIZE>::import_from_der(
  const byte_buffer_ref key
  )
{
  DWORD blob_size = sizeof(_blob);
  CryptDecodeObject(
    X509_ASN_ENCODING,
    RSA_CSP_PUBLICKEYBLOB,
    key.get_buffer(),
    (DWORD)key.get_size(),
    0,
    (BYTE*)&_blob,
    &blob_size);

  this->import_from_blob(_blob);
}

template <
  size_type KEY_SIZE
>
void
rsa_public_key<KEY_SIZE>::import_from_pem(
  const string_ref key
  )
{
  return import_from_der(base64::decode(key));
}


template <
  size_type KEY_SIZE
>
byte_buffer
rsa_public_key<KEY_SIZE>::encrypt(
  const byte_buffer_ref input,
  rsa_encryption_padding padding,
  bool do_final
  )
{
  byte_buffer output(key_size_in_bytes);
  input.copy_to(output);

  DWORD buffer_size = static_cast<DWORD>(input.get_size());
  CryptEncrypt(
    get_handle(),
    0,
    do_final,
    detail::rsa_encryption_padding_map[static_cast<int>(padding)],
    output.get_buffer(),
    &buffer_size,
    key_size_in_bytes);

  output.resize(buffer_size);
  memory::reverse(output.get_buffer(), output.get_size());

  return output;
}

//
// RSA
// private key.
//

template <
  size_type KEY_SIZE
>
rsa_private_key<KEY_SIZE>::rsa_private_key(
  rsa_private_key&& other
  )
{
  swap(other);
}

template <
  size_type KEY_SIZE
>
rsa_private_key<KEY_SIZE>&
rsa_private_key<KEY_SIZE>::operator=(
  rsa_private_key&& other
  )
{
  swap(other);
  return *this;
}

template <
  size_type KEY_SIZE
>
void
rsa_private_key<KEY_SIZE>::swap(
  rsa_private_key& other
  )
{
  key::swap(other);
  mini::swap(_blob, other._blob);
}

//
// import.
//

template <
  size_type KEY_SIZE
>
rsa_private_key<KEY_SIZE>
rsa_private_key<KEY_SIZE>::make_from_der(
  const byte_buffer_ref key
  )
{
  rsa_private_key<KEY_SIZE> result;
  result.import_from_der(key);
  return result;
}

template <
  size_type KEY_SIZE
>
rsa_private_key<KEY_SIZE>
rsa_private_key<KEY_SIZE>::make_from_pem(
  const string_ref key
  )
{
  return make_from_der(base64::decode(key));
}

template <
  size_type KEY_SIZE
>
void
rsa_private_key<KEY_SIZE>::import_from_der(
  const byte_buffer_ref key
  )
{
  DWORD blob_size = sizeof(_blob);
  CryptDecodeObject(
    X509_ASN_ENCODING,
    PKCS_RSA_PRIVATE_KEY,
    key.get_buffer(),
    (DWORD)key.get_size(),
    0,
    (BYTE*)&_blob,
    &blob_size);

  this->import_from_blob(_blob);
}

template <
  size_type KEY_SIZE
>
void
rsa_private_key<KEY_SIZE>::import_from_pem(
  const string_ref key
  )
{
  return import_from_der(base64::decode(key));
}

template <
  size_type KEY_SIZE
>
rsa_public_key<KEY_SIZE>
rsa_private_key<KEY_SIZE>::export_public_key(
  void
  ) const
{
  rsa_public_key<KEY_SIZE> result;
  this->export_to_blob(PUBLICKEYBLOB, result._blob);
  result.import_from_blob(result._blob);
  return result;
}


template <
  size_type KEY_SIZE
>
byte_buffer
rsa_private_key<KEY_SIZE>::decrypt(
  const byte_buffer_ref input,
  rsa_encryption_padding padding,
  bool do_final
  )
{
  byte_buffer output(key_size_in_bytes);
  input.copy_to(output);
  memory::reverse(output.get_buffer(), output.get_size());

  DWORD buffer_size = static_cast<DWORD>(input.get_size());
  CryptDecrypt(
    get_handle(),
    0,
    do_final,
    detail::rsa_encryption_padding_map[static_cast<int>(padding)],
    output.get_buffer(),
    &buffer_size);

  output.resize(buffer_size);

  return output;
}

}
