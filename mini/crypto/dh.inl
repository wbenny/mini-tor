#include "dh.h"
#include "random.h"

namespace mini::crypto {

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
  // pre-allocate.
  //
  : _y(key_size_in_bytes)
{

}

template <
  size_type KEY_SIZE
>
dh_public_key<KEY_SIZE>::dh_public_key(
  const byte_buffer_ref y,
  little_endian_tag
  )
  : dh_public_key<key_size>()
{
  memory::reverse_copy(&_y[0], y.get_buffer(), key_size_in_bytes);
}

template <
  size_type KEY_SIZE
>
dh_public_key<KEY_SIZE>::dh_public_key(
  const byte_buffer_ref y,
  big_endian_tag
  )
  : dh_public_key<key_size>()
{
  memory::copy(&_y[0], y.get_buffer(), key_size_in_bytes);
}

template <
  size_type KEY_SIZE
>
dh_public_key<KEY_SIZE>::dh_public_key(
  const byte_buffer_ref y
  )
  : dh_public_key<key_size>(y, big_endian_tag())
{

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
  mini::swap(_y, other._y);
}

//
// import.
//

template <
  size_type KEY_SIZE
>
void
dh_public_key<KEY_SIZE>::import(
  const byte_buffer_ref y
  )
{
  import(y, big_endian_tag());
}

template <
  size_type KEY_SIZE
>
void
dh_public_key<KEY_SIZE>::import(
  const byte_buffer_ref y,
  little_endian_tag
  )
{
  memory::reverse_copy(&_y[0], y.get_buffer(), key_size_in_bytes);
}

template <
  size_type KEY_SIZE
>
void
dh_public_key<KEY_SIZE>::import(
  const byte_buffer_ref y,
  big_endian_tag
  )
{
  memory::copy(&_y[0], y.get_buffer(), key_size_in_bytes);
}

//
// getters.
//

template <
  size_type KEY_SIZE
>
byte_buffer
dh_public_key<KEY_SIZE>::get_y(
  void
  ) const
{
  return _y;
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
  // pre-allocate.
  //
  : _generator(key_size_in_bytes)
  , _modulus(key_size_in_bytes)
  , _exponent(key_size_in_bytes)
{

}

template <
  size_type KEY_SIZE
>
dh_private_key<KEY_SIZE>::dh_private_key(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus,
  const byte_buffer_ref exponent
  ) : dh_private_key<key_size>(generator, modulus, exponent, big_endian_tag())
{

}

template <
  size_type KEY_SIZE
>
dh_private_key<KEY_SIZE>::dh_private_key(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus,
  const byte_buffer_ref exponent,
  little_endian_tag
  ) : dh_private_key<key_size>()
{
  import(generator, modulus, exponent, little_endian_tag());
}

template <
  size_type KEY_SIZE
>
dh_private_key<KEY_SIZE>::dh_private_key(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus,
  const byte_buffer_ref exponent,
  big_endian_tag
  ) : dh_private_key<KEY_SIZE>()
{
  import(generator, modulus, exponent, big_endian_tag());
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
  mini::swap(_generator, other._generator);
  mini::swap(_modulus,   other._modulus);
  mini::swap(_exponent,  other._exponent);
}

//
// import.
//

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
  import(generator, modulus, exponent, big_endian_tag());
}

template <
  size_type KEY_SIZE
>
void
dh_private_key<KEY_SIZE>::import(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus,
  const byte_buffer_ref exponent,
  little_endian_tag
  )
{
  generator.reverse_copy_to(_generator);
  modulus.reverse_copy_to(_modulus);
  exponent.reverse_copy_to(_exponent);

  import_impl();
}

template <
  size_type KEY_SIZE
>
void
dh_private_key<KEY_SIZE>::import(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus,
  const byte_buffer_ref exponent,
  big_endian_tag
  )
{
  mutable_byte_buffer_ref(_generator)
    .slice((key_size_in_bytes) - generator.get_size())
    .copy_from(generator);

  mutable_byte_buffer_ref(_modulus)
    .slice((key_size_in_bytes) - modulus.get_size())
    .copy_from(modulus);

  mutable_byte_buffer_ref(_exponent)
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
  typename dh_public_key<KEY_SIZE>::blob public_key_blob;
  this->export_to_blob(PUBLICKEYBLOB, public_key_blob);

  return dh_public_key<key_size>(
    public_key_blob.y,
    little_endian_tag());
}

//
// getters.
//

template <
  size_type KEY_SIZE
>
byte_buffer
dh_private_key<KEY_SIZE>::get_generator(
  void
  ) const
{
  return _generator;
}

template <
  size_type KEY_SIZE
>
byte_buffer
dh_private_key<KEY_SIZE>::get_modulus(
  void
  ) const
{
  return _modulus;
}

template <
  size_type KEY_SIZE
>
byte_buffer
dh_private_key<KEY_SIZE>::get_exponent(
  void
  ) const
{
  return _exponent;
}

template <
  size_type KEY_SIZE
>
byte_buffer
dh_private_key<KEY_SIZE>::get_shared_secret(
  const dh_public_key<KEY_SIZE>& other_public_key
  ) const
{
  //
  // yolo.
  //

  return dh_private_key<KEY_SIZE>(
    other_public_key.get_y(),
    _modulus,
    _exponent).export_public_key().get_y();
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
  // destroy previous key.
  //
  this->destroy();

  blob private_key_blob = {
    { PRIVATEKEYBLOB, CUR_BLOB_VERSION, 0, CALG_DH_EPHEM },
    { DH_PRIVATE_MAGIC, key_size }
  };

  //
  // big endian -> little endian
  //
  memory::reverse_copy(
    private_key_blob.generator,
    _generator.get_buffer(),
    _generator.get_size());

  memory::reverse_copy(
    private_key_blob.prime,
    _modulus.get_buffer(),
    _modulus.get_size());

  memory::reverse_copy(
    private_key_blob.secret,
    _exponent.get_buffer(),
    _exponent.get_size());

  this->import_from_blob(private_key_blob);
}

//
// DH
//

template <
  size_type KEY_SIZE
>
typename dh<KEY_SIZE>::private_key
dh<KEY_SIZE>::generate_private_key(
  const byte_buffer_ref generator,
  const byte_buffer_ref modulus
  )
{
  //
  // generate private key.
  //
  return private_key(
    generator,
    modulus,
    random_device.get_random_bytes(key_size_in_bytes));
}

}
