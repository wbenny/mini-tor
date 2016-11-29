#include "hash_base.h"
#include "provider.h"

namespace mini::crypto {

//
// constructors.
//

hash_base::hash_base(
  ALG_ID alg_id
  )
{
  init(alg_id);
}

hash_base::hash_base(
  const hash_base& other
  )
{
  duplicate_internal(other);
}

hash_base::hash_base(
  hash_base&& other
  )
{
  swap(other);
}

//
// destructor.
//

hash_base::~hash_base(
  void
  )
{
  destroy();
}

//
// assign operators.
//

hash_base&
hash_base::operator=(
  const hash_base& other
  )
{
  duplicate_internal(other);
  return *this;
}

hash_base&
hash_base::operator=(
  hash_base&& other
  )
{
  swap(other);
  return *this;
}

//
// swap.
//

void
hash_base::swap(
  hash_base& other
  )
{
  mini::swap(_hash_handle, other._hash_handle);
}

//
// operations.
//

void
hash_base::update(
  const byte_buffer_ref input
  )
{
  CryptHashData(
    _hash_handle,
    input.get_buffer(),
    static_cast<DWORD>(input.get_size()),
    0);
}

//
// accessors.
//

void
hash_base::get(
  mutable_byte_buffer_ref output
  )
{
  mini_assert(output.get_size() >= get_hash_size());

  DWORD hash_size = static_cast<DWORD>(get_hash_size());

  CryptGetHashParam(
    _hash_handle,
    HP_HASHVAL,
    output.get_buffer(),
    &hash_size,
    0);
}

byte_buffer
hash_base::get(
  void
  )
{
  byte_buffer result(get_hash_size());
  get(result);

  return result;
}

//
// protected methods.
//

void
hash_base::init(
  ALG_ID alg_id
  )
{
  CryptCreateHash(
    provider_factory.get_rsa_aes_handle(),
    alg_id,
    0,
    0,
    &_hash_handle);
}

void
hash_base::destroy(
  void
  )
{
  if (_hash_handle)
  {
    CryptDestroyHash(_hash_handle);
  }
}

void
hash_base::duplicate_internal(
  const hash_base& other
  )
{
  //
  // destroy previous hash.
  //
  destroy();

  CryptDuplicateHash(
    other._hash_handle,
    NULL,
    0,
    &_hash_handle);
}

}
