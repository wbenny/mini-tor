#include "dh.h"
#include "../random.h"

namespace mini::crypto::cng {

//
// DH
// public key.
//

template <
  size_type KEY_SIZE
>
dh_public_key<KEY_SIZE>::dh_public_key(
  void
  )
  : _blob({
    { BCRYPT_DH_PUBLIC_MAGIC, key_size_in_bytes },
  })
{

}

template <
  size_type KEY_SIZE
>
dh_public_key<KEY_SIZE>::dh_public_key(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus,
  const byte_buffer_ref y
  )
  : dh_public_key<KEY_SIZE>()
{
  import(generator, modulus, y);
}

template <
  size_type KEY_SIZE
>
dh_public_key<KEY_SIZE>::dh_public_key(
  dh_public_key&& other
  )
{
  swap(other);
}

template <
  size_type KEY_SIZE
>
dh_public_key<KEY_SIZE>&
dh_public_key<KEY_SIZE>::operator=(
  dh_public_key&& other
  )
{
  swap(other);
  return *this;
}

template <
  size_type KEY_SIZE
>
void
dh_public_key<KEY_SIZE>::swap(
  dh_public_key& other
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
void
dh_public_key<KEY_SIZE>::import(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus,
  const byte_buffer_ref y
  )
{
  mutable_byte_buffer_ref(_blob.generator)
    .slice((key_size_in_bytes) - generator.get_size())
    .copy_from(generator);

  mutable_byte_buffer_ref(_blob.prime)
    .slice((key_size_in_bytes) - modulus.get_size())
    .copy_from(modulus);

  mutable_byte_buffer_ref(_blob.y)
    .slice((key_size_in_bytes) - y.get_size())
    .copy_from(y);

  this->import_from_blob(_blob);
}

//
// getters.
//

template <
  size_type KEY_SIZE
>
byte_buffer_ref
dh_public_key<KEY_SIZE>::get_generator(
  void
  ) const
{
  return _blob.generator;
}

template <
  size_type KEY_SIZE
>
byte_buffer_ref
dh_public_key<KEY_SIZE>::get_modulus(
  void
  ) const
{
  return _blob.prime;
}

template <
  size_type KEY_SIZE
>
byte_buffer_ref
dh_public_key<KEY_SIZE>::get_y(
  void
  ) const
{
  return _blob.y;
}

//
// DH
// private key
//

template <
  size_type KEY_SIZE
>
dh_private_key<KEY_SIZE>::dh_private_key(
  void
  )
  : _blob({
    { BCRYPT_DH_PRIVATE_MAGIC, key_size_in_bytes },
  })
{

}

template <
  size_type KEY_SIZE
>
dh_private_key<KEY_SIZE>::dh_private_key(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus,
  const byte_buffer_ref exponent
  )
  //
  // it is important to call the ctor here,
  // because it builds the blob header.
  //
  : dh_private_key<KEY_SIZE>()
{
  import(generator, modulus, exponent);
}

template <
  size_type KEY_SIZE
>
dh_private_key<KEY_SIZE>::dh_private_key(
  dh_private_key&& other
  )
{
  swap(other);
}

template <
  size_type KEY_SIZE
>
dh_private_key<KEY_SIZE>&
dh_private_key<KEY_SIZE>::operator=(
  dh_private_key&& other
  )
{
  swap(other);
  return *this;
}

template <
  size_type KEY_SIZE
>
void
dh_private_key<KEY_SIZE>::swap(
  dh_private_key& other
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
dh_private_key<KEY_SIZE>
dh_private_key<KEY_SIZE>::generate(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus
  )
{
  //
  // generate private key.
  //
  return dh_private_key<KEY_SIZE>(
    generator,
    modulus,
    random_device.get_random_bytes(key_size_in_bytes));
}

template <
  size_type KEY_SIZE
>
void
dh_private_key<KEY_SIZE>::import(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus,
  const byte_buffer_ref exponent
  )
{
  mutable_byte_buffer_ref(_blob.generator)
    .slice((key_size_in_bytes) - generator.get_size())
    .copy_from(generator);

  mutable_byte_buffer_ref(_blob.prime)
    .slice((key_size_in_bytes) - modulus.get_size())
    .copy_from(modulus);

  mutable_byte_buffer_ref(_blob.secret)
    .slice((key_size_in_bytes) - exponent.get_size())
    .copy_from(exponent);

  this->import_from_blob(_blob);

  //
  // import computed the "y" (public key),
  // export it to actualize the cached blob.
  //
  this->export_to_blob(_blob, BCRYPT_DH_PRIVATE_BLOB);
}

//
// export.
//

template <
  size_type KEY_SIZE
>
dh_public_key<KEY_SIZE>
dh_private_key<KEY_SIZE>::export_public_key(
  void
  ) const
{
  dh_public_key<KEY_SIZE> result;
  this->export_to_blob(result._blob, BCRYPT_DH_PUBLIC_BLOB);
  result.import_from_blob(result._blob);
  return result;
}

//
// getters.
//

template <
  size_type KEY_SIZE
>
byte_buffer_ref
dh_private_key<KEY_SIZE>::get_generator(
  void
  ) const
{
  return _blob.generator;
}

template <
  size_type KEY_SIZE
>
byte_buffer_ref
dh_private_key<KEY_SIZE>::get_modulus(
  void
  ) const
{
  return _blob.prime;
}

template <
  size_type KEY_SIZE
>
byte_buffer_ref
dh_private_key<KEY_SIZE>::get_exponent(
  void
  ) const
{
  return _blob.secret;
}

template <
  size_type KEY_SIZE
>
byte_buffer_ref
dh_private_key<KEY_SIZE>::get_y(
  void
  ) const
{
  return _blob.y;
}

template <
  size_type KEY_SIZE
>
byte_buffer
dh_private_key<KEY_SIZE>::get_shared_secret(
  const dh_public_key<KEY_SIZE>& other_public_key
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

template <
  size_type KEY_SIZE
>
byte_buffer
dh_private_key<KEY_SIZE>::get_shared_secret(
  const byte_buffer_ref other_public_key_y
  ) const
{
  return get_shared_secret(dh_public_key<KEY_SIZE>(
    get_generator(),
    get_modulus(),
    other_public_key_y));
}

}
