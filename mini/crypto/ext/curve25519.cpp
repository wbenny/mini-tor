#include "curve25519.h"
#include "detail/curve25519-donna.h"
#include "../random.h"

namespace mini::crypto::ext {

static const uint8_t basepoint_9[32] = { 9 };

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
  memory::copy(_blob.X, key.get_buffer(), key_size_in_bytes);
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
  memory::copy(_blob.d, key.get_buffer(), key_size_in_bytes);
  detail::curve25519_donna(_blob.X, _blob.d, basepoint_9);
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
  detail::curve25519_donna(result._blob.X, _blob.d, basepoint_9);
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
  byte_buffer result(key_size_in_bytes);
  detail::curve25519_donna(&result[0], _blob.d, other_public_key._blob.X);
  return result;
}

}
