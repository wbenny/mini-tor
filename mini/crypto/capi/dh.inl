#include "dh.h"
#include "random.h"

namespace mini::crypto::capi {

#define DH_PUBLIC_MAGIC  0x31484400
#define DH_PRIVATE_MAGIC 0x32484400

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
  //
  // just zero _blob out, we don't actually need the header.
  //
  : _blob()
  //
  // but in case anyone is curious, the header should look like this:
  //
  // {
  //   { PUBLICKEYBLOB, CUR_BLOB_VERSION, 0, CALG_DH_EPHEM },
  //   { DH_PUBLIC_MAGIC, key_size }
  // }
  //
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
  mini::swap(_generator, other._generator);
  mini::swap(_prime, other._prime);
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
  mutable_byte_buffer_ref(_generator)
    .slice((key_size_in_bytes) - generator.get_size())
    .copy_from(generator);

  mutable_byte_buffer_ref(_prime)
    .slice((key_size_in_bytes) - modulus.get_size())
    .copy_from(modulus);

  mutable_byte_buffer_ref(_blob.y)
    .slice((key_size_in_bytes) - y.get_size())
    .copy_from(y);

  //
  // i couldn't figure out how to import DH public key via CryptoAPI.
  // luckily, we don't need the handle for the key anyway if we want to
  // compute shared secret.
  //
  // this->import_from_blob(_blob);
  //
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
  return _generator;
}

template <
  size_type KEY_SIZE
>
byte_buffer_ref
dh_public_key<KEY_SIZE>::get_modulus(
  void
  ) const
{
  return _prime;
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
  //
  // build the blob header now so we won't
  // have to touch it anywhere else.
  //
  : _blob({
    { PRIVATEKEYBLOB, CUR_BLOB_VERSION, 0, CALG_DH_EPHEM },
    { DH_PRIVATE_MAGIC, key_size }
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
  _public_key.swap(other._public_key);
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

  import_impl();
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
  dh_public_key<key_size> result;

  //
  // this call will place the PUBLICKEYBLOB header
  // into the dh_public_key::_blob along with "y".
  //
  this->export_to_blob(result._blob, PUBLICKEYBLOB);

  //
  // little endian -> big endian
  //
  memory::reverse(result._blob.y, key_size_in_bytes);

  // result.import_from_blob(result._blob);

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
  return _public_key._blob.y;
}

template <
  size_type KEY_SIZE
>
byte_buffer
dh_private_key<KEY_SIZE>::get_shared_secret(
  const dh_public_key<KEY_SIZE>& other_public_key
  ) const
{
//   mini_assert(get_generator() == other_public_key.get_generator());
//   mini_assert(get_modulus()   == other_public_key.get_modulus());

  return get_shared_secret(other_public_key.get_y());
}

template <
  size_type KEY_SIZE
>
byte_buffer
dh_private_key<KEY_SIZE>::get_shared_secret(
  const byte_buffer_ref other_public_key_y
  ) const
{
  //
  // yolo.
  //

  return dh_private_key<KEY_SIZE>(
    other_public_key_y,
    _blob.prime,
    _blob.secret).export_public_key().get_y();
}

template <
  size_type KEY_SIZE
>
void
dh_private_key<KEY_SIZE>::import_impl(
  void
  )
{
  //
  // big endian -> little endian
  //
  memory::reverse(_blob.generator, key_size_in_bytes);
  memory::reverse(_blob.prime,     key_size_in_bytes);
  memory::reverse(_blob.secret,    key_size_in_bytes);

  this->import_from_blob(_blob);

  //
  // little endian -> big endian
  //
  memory::reverse(_blob.generator, key_size_in_bytes);
  memory::reverse(_blob.prime,     key_size_in_bytes);
  memory::reverse(_blob.secret,    key_size_in_bytes);

  _public_key = export_public_key();
}

}
