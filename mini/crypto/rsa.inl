#include "rsa.h"
#include "base64.h"

namespace mini::crypto {

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
  this->swap(other);
}

template <
  size_type KEY_SIZE
>
rsa_public_key<KEY_SIZE>&
rsa_public_key<KEY_SIZE>::operator=(
  rsa_public_key&& other
  )
{
  this->swap(other);
  return *this;
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
  blob public_key_blob;
  DWORD public_key_blob_size = sizeof(public_key_blob);

  CryptDecodeObject(
    X509_ASN_ENCODING,
    RSA_CSP_PUBLICKEYBLOB,
    key.get_buffer(),
    (DWORD)key.get_size(),
    0,
    (BYTE*)&public_key_blob,
    &public_key_blob_size);

  this->import_from_blob(public_key_blob);
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
  this->swap(other);
}

template <
  size_type KEY_SIZE
>
rsa_private_key<KEY_SIZE>&
rsa_private_key<KEY_SIZE>::operator=(
  rsa_private_key&& other
  )
{
  this->swap(other);
  return *this;
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
  //
  // destroy previous key.
  //
  this->destroy();

  blob private_key_blob;
  DWORD private_key_blob_size = sizeof(private_key_blob);

  CryptDecodeObject(
    X509_ASN_ENCODING,
    PKCS_RSA_PRIVATE_KEY,
    key.get_buffer(),
    (DWORD)key.get_size(),
    0,
    (BYTE*)&private_key_blob,
    &private_key_blob_size);

  this->import_from_blob(private_key_blob);
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
  typename rsa_public_key<KEY_SIZE>::blob public_key_blob;
  this->export_to_blob(PUBLICKEYBLOB, public_key_blob);

  rsa_public_key<KEY_SIZE> result;
  result.import_from_blob(public_key_blob);
  return result;
}

//
// RSA
//

template <
  size_type KEY_SIZE
>
rsa<KEY_SIZE>::rsa(
  void
  )
{

};

template <
  size_type KEY_SIZE
>
rsa<KEY_SIZE>::rsa(
  public_key&& key
  )
  : _public_key(std::move(key))
{

}

template <
  size_type KEY_SIZE
>
rsa<KEY_SIZE>::rsa(
  private_key&& key
  )
  : _private_key(std::move(key))
{

}

template <
  size_type KEY_SIZE
>
rsa<KEY_SIZE>::~rsa(
  void
  )
{
  destroy_key();
}

template <
  size_type KEY_SIZE
>
void
rsa<KEY_SIZE>::set_public_key(
  public_key&& key
  )
{
  destroy_key();
  _public_key = std::move(key);
}

template <
  size_type KEY_SIZE
>
void
rsa<KEY_SIZE>::set_private_key(
  private_key&& key
  )
{
  destroy_key();
  _private_key = std::move(key);
}

template <
  size_type KEY_SIZE
>
byte_buffer
rsa<KEY_SIZE>::public_encrypt(
  const byte_buffer_ref input,
  encryption_padding padding,
  bool do_final
  )
{
  byte_buffer output;

  if (_public_key)
  {
    output.resize(key_size_in_bytes);
    input.copy_to(output);

    DWORD dword_output_size = static_cast<DWORD>(input.get_size());

    CryptEncrypt(
      _public_key.get_handle(),
      0,
      do_final,
      static_cast<DWORD>(padding),
      output.get_buffer(),
      &dword_output_size,
      key_size_in_bytes);

    output.resize(dword_output_size);
    memory::reverse(output.get_buffer(), output.get_size());
  }

  return output;
}

template <
  size_type KEY_SIZE
>
byte_buffer
rsa<KEY_SIZE>::private_decrypt(
  const byte_buffer_ref input,
  encryption_padding padding,
  bool do_final
  )
{
  byte_buffer output;

  if (_private_key)
  {
    output.resize(key_size_in_bytes);
    input.copy_to(output);

    memory::reverse(output.get_buffer(), output.get_size());

    DWORD dword_output_size = static_cast<DWORD>(input.get_size());

    CryptDecrypt(
      _private_key.get_handle(),
      0,
      do_final,
      static_cast<DWORD>(padding),
      output.get_buffer(),
      &dword_output_size);

    output.resize(dword_output_size);
  }

  return output;
}

template <
  size_type KEY_SIZE
>
byte_buffer
rsa<KEY_SIZE>::public_encrypt(
  public_key&& key,
  const byte_buffer_ref input,
  encryption_padding padding
  )
{
  return rsa<KEY_SIZE>(std::move(key))
    .public_encrypt(input, padding, true);
}

template <
  size_type KEY_SIZE
>
byte_buffer
rsa<KEY_SIZE>::private_decrypt(
  private_key&& key,
  const byte_buffer_ref input,
  encryption_padding padding
  )
{
  return rsa<KEY_SIZE>(std::move(key))
    .private_decrypt(input, padding, true);
}

template <
  size_type KEY_SIZE
>
void
rsa<KEY_SIZE>::destroy_key(
  void
  )
{
  _key.destroy();
}

}
