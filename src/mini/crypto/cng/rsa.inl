#include "rsa.h"
#include "../capi/rsa.h" // for CryptoAPI key blob definitions.
#include "../base64.h" // for conversion between der & pem.

namespace mini::crypto::cng {

namespace detail {

  //
  // map each value from rsa_encryption_padding to its
  // corresponding CryptoAPI definition.
  //
  static constexpr ULONG rsa_encryption_padding_map[] = {
    BCRYPT_PAD_PKCS1,
    BCRYPT_PAD_OAEP,
  };

}

//
// RSA
// public key.
//

template <
  size_type KEY_SIZE
>
rsa_public_key<KEY_SIZE>::rsa_public_key(
  void
  )
  : _blob({
    { BCRYPT_RSAPUBLIC_MAGIC
    , key_size
    , mini_sizeof_struct_member(blob, publicExponent)
    , mini_sizeof_struct_member(blob, modulus)
    , 0
    , 0 }
  })
{

}

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
  typename capi::rsa_public_key<KEY_SIZE>::blob capi_blob;
  DWORD capi_blob_size = sizeof(capi_blob);
  CryptDecodeObject(
    X509_ASN_ENCODING,
    RSA_CSP_PUBLICKEYBLOB,
    key.get_buffer(),
    (DWORD)key.get_size(),
    0,
    (BYTE*)&capi_blob,
    &capi_blob_size);

  //
  // little-endian (CryptoAPI) -> big-endian (CNG)
  //

  memory::reverse_copy(_blob.publicExponent, &capi_blob.rsapubkey.pubexp, sizeof(capi_blob.rsapubkey.pubexp));
  memory::reverse_copy(_blob.modulus, capi_blob.modulus, sizeof(capi_blob.modulus));

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
  MINI_UNREFERENCED(do_final);

  byte_buffer output(key_size_in_bytes);
  input.copy_to(output);

  BCRYPT_OAEP_PADDING_INFO padding_info;
  void* padding_info_ptr = nullptr;

  //
  // use padding info for OAEP_SHA1.
  // do not use it for PKCS1.
  //
  if (padding == rsa_encryption_padding::oaep_sha1)
  {
    padding_info = { BCRYPT_SHA1_ALGORITHM };
    padding_info_ptr = &padding_info;
  }

  ULONG buffer_size;
  BCryptEncrypt(
    get_handle(),
    const_cast<PUCHAR>(input.get_buffer()),
    static_cast<ULONG>(input.get_size()),
    padding_info_ptr,
    nullptr,
    0,
    static_cast<PUCHAR>(output.get_buffer()),
    static_cast<ULONG>(output.get_size()),
    &buffer_size,
    detail::rsa_encryption_padding_map[static_cast<int>(padding)]);

  output.resize(buffer_size);

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
  void
  )
  : _blob({
    { BCRYPT_RSAFULLPRIVATE_MAGIC
    , key_size
    , mini_sizeof_struct_member(blob, publicExponent)
    , mini_sizeof_struct_member(blob, modulus)
    , mini_sizeof_struct_member(blob, prime1)
    , mini_sizeof_struct_member(blob, prime2) }
  })
{

}

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
  typename capi::rsa_private_key<KEY_SIZE>::blob capi_blob;
  DWORD capi_blob_size = sizeof(capi_blob);
  CryptDecodeObject(
    X509_ASN_ENCODING,
    PKCS_RSA_PRIVATE_KEY,
    key.get_buffer(),
    (DWORD)key.get_size(),
    0,
    (BYTE*)&capi_blob,
    &capi_blob_size);

  //
  // little-endian (CryptoAPI) -> big-endian (CNG)
  //

  memory::reverse_copy(_blob.publicExponent, &capi_blob.rsapubkey.pubexp, sizeof(capi_blob.rsapubkey.pubexp));
  memory::reverse_copy(_blob.modulus, capi_blob.modulus, sizeof(capi_blob.modulus));

  memory::reverse_copy(_blob.prime1, capi_blob.prime1, sizeof(capi_blob.prime1));
  memory::reverse_copy(_blob.prime2, capi_blob.prime2, sizeof(capi_blob.prime2));
  memory::reverse_copy(_blob.exponent1, capi_blob.exponent1, sizeof(capi_blob.exponent1));
  memory::reverse_copy(_blob.exponent2, capi_blob.exponent2, sizeof(capi_blob.exponent2));
  memory::reverse_copy(_blob.coefficient, capi_blob.coefficient, sizeof(capi_blob.coefficient));
  memory::reverse_copy(_blob.privateExponent, capi_blob.privateExponent, sizeof(capi_blob.privateExponent));

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
  typename rsa_public_key<KEY_SIZE>::blob public_key_blob;
  this->export_to_blob(BCRYPT_RSAPUBLIC_BLOB, public_key_blob);

  rsa_public_key<KEY_SIZE> result;
  result.import_from_blob(public_key_blob);
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
  MINI_UNREFERENCED(do_final);

  byte_buffer output(key_size_in_bytes);
  input.copy_to(output);

  BCRYPT_OAEP_PADDING_INFO padding_info;
  void* padding_info_ptr = nullptr;

  //
  // use padding info for OAEP_SHA1.
  // do not use it for PKCS1.
  //
  if (padding == rsa_encryption_padding::oaep_sha1)
  {
    padding_info = { BCRYPT_SHA1_ALGORITHM };
    padding_info_ptr = &padding_info;
  }

  ULONG buffer_size;
  BCryptDecrypt(
    get_handle(),
    const_cast<PUCHAR>(input.get_buffer()),
    static_cast<ULONG>(input.get_size()),
    padding_info_ptr,
    nullptr,
    0,
    static_cast<PUCHAR>(output.get_buffer()),
    static_cast<ULONG>(output.get_size()),
    &buffer_size,
    detail::rsa_encryption_padding_map[static_cast<int>(padding)]);

  output.resize(buffer_size);

  return output;
}

}
