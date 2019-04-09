#include "curve25519.h"
#include "../random.h"

namespace mini::crypto::cng {

//
// curve25519
// public key.
//

curve25519_public_key::curve25519_public_key(
  const byte_buffer_ref key
  )
{
  import(key);
}

curve25519_public_key::curve25519_public_key(
  curve25519_public_key&& other
  )
{
  swap(other);
}

curve25519_public_key&
curve25519_public_key::operator=(
  curve25519_public_key&& other
  )
{
  swap(other);
  return *this;
}

void
curve25519_public_key::swap(
  curve25519_public_key& other
  )
{
  key::swap(other);
  mini::swap(_blob, other._blob);
}

//
// import.
//

void
curve25519_public_key::import(
  const byte_buffer_ref key
  )
{
  _blob = {
    { BCRYPT_ECDH_PUBLIC_GENERIC_MAGIC, key_size_in_bytes }
  };

  memory::copy(_blob.X, key.get_buffer(), _blob.ecckeyblob.cbKey);

  this->import_from_blob(_blob);
}

//
// getters.
//

byte_buffer
curve25519_public_key::get_public_key_buffer(
  void
) const
{
  return _blob.X;
}

//
// curve25519
// private key.
//

curve25519_private_key::curve25519_private_key(
  const byte_buffer_ref key
  )
{
  import(key);
}

curve25519_private_key::curve25519_private_key(
  curve25519_private_key&& other
  )
{
  swap(other);
}

curve25519_private_key&
curve25519_private_key::operator=(
  curve25519_private_key&& other
  )
{
  swap(other);
  return *this;
}

void
curve25519_private_key::swap(
  curve25519_private_key& other
  )
{
  key::swap(other);
  mini::swap(_blob, other._blob);
}

//
// import.
//

curve25519_private_key
curve25519_private_key::generate(
  void
  )
{
  auto random_private_key = random_device.get_random_bytes(key_size_in_bytes);
  random_private_key[0]  &= 248;
  random_private_key[31] &= 127;
  random_private_key[31] |= 64;

  return curve25519_private_key(random_private_key);
}

void
curve25519_private_key::import(
  const byte_buffer_ref key
  )
{
  _blob = {
    { BCRYPT_ECDH_PRIVATE_GENERIC_MAGIC, key_size_in_bytes }
  };

  memory::copy(_blob.d, key.get_buffer(), _blob.ecckeyblob.cbKey);

  this->import_from_blob(_blob);

  //
  // import computed the "X" (public key),
  // export it to actualize the cached blob.
  //
  this->export_to_blob(_blob, BCRYPT_ECCPRIVATE_BLOB);
}

//
// export.
//

curve25519_public_key
curve25519_private_key::export_public_key(
  void
  ) const
{
  curve25519_public_key result;
  this->export_to_blob(result._blob, BCRYPT_ECCPUBLIC_BLOB);
  result.import_from_blob(result._blob);
  return result;
}

//
// getters.
//

byte_buffer_ref
curve25519_private_key::get_public_key_buffer(
  void
  ) const
{
  return _blob.X;
}

byte_buffer_ref
curve25519_private_key::get_private_key_buffer(
  void
  ) const
{
  return _blob.d;
}

byte_buffer
curve25519_private_key::get_shared_secret(
  const curve25519_public_key& other_public_key
  ) const
{
  BCRYPT_SECRET_HANDLE shared_secret_handle;
  BCryptSecretAgreement(
    get_handle(),
    other_public_key.get_handle(),
    &shared_secret_handle,
    0);

  byte_buffer result(key_size_in_bytes);
  DWORD result_size = key_size_in_bytes;

  BCryptDeriveKey(
    shared_secret_handle,
    BCRYPT_KDF_RAW_SECRET,
    NULL,
    result.get_buffer(),
    result_size,
    &result_size,
    NULL);

  BCryptDestroySecret(shared_secret_handle);

  //
  // BCRYPT_KDF_RAW_SECRET is supported on Win8.1+
  // and it provides shared key in little-endian
  // for some obscure reason.
  //
  memory::reverse(result.get_buffer(), result.get_size());

  return result;
}

}
