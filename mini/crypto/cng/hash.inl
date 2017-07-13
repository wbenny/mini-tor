#include "hash.h"
#include "provider.h"

namespace mini::crypto::cng {

//
// constructors.
//

template <
  hash_algorithm_type HASH_ALGORITHM
>
hash<HASH_ALGORITHM>::hash(
  void
  )
{
  static constexpr auto alg_handle =
    HASH_ALGORITHM == hash_algorithm_type::sha1 ? &provider::get_hash_sha1_handle :
    nullptr;

  init((provider_factory.*alg_handle)());
}

template <
  hash_algorithm_type HASH_ALGORITHM
>
hash<HASH_ALGORITHM>::hash(
  const hash& other
  )
{
  duplicate_internal(other);
}

template <
  hash_algorithm_type HASH_ALGORITHM
>
hash<HASH_ALGORITHM>::hash(
  hash&& other
  )
{
  swap(other);
}

template <
  hash_algorithm_type HASH_ALGORITHM
>
hash<HASH_ALGORITHM>::hash(
  BCRYPT_ALG_HANDLE alg_handle,
  const byte_buffer_ref key
  )
{
  init(alg_handle, key);
}

//
// destructor.
//

template <
  hash_algorithm_type HASH_ALGORITHM
>
hash<HASH_ALGORITHM>::~hash(
  void
  )
{
  destroy();
}

//
// assign operators.
//

template <
  hash_algorithm_type HASH_ALGORITHM
>
hash<HASH_ALGORITHM>&
hash<HASH_ALGORITHM>::operator=(
  const hash& other
  )
{
  duplicate_internal(other);
  return *this;
}

template <
  hash_algorithm_type HASH_ALGORITHM
>
hash<HASH_ALGORITHM>&
hash<HASH_ALGORITHM>::operator=(
  hash&& other
  )
{
  swap(other);
  return *this;
}

template <
  hash_algorithm_type HASH_ALGORITHM
>
hash<HASH_ALGORITHM>
hash<HASH_ALGORITHM>::duplicate(
  void
  )
{
  return hash<HASH_ALGORITHM>(*this);
}

//
// swap.
//

template <
  hash_algorithm_type HASH_ALGORITHM
>
void
hash<HASH_ALGORITHM>::swap(
  hash& other
  )
{
  mini::swap(_hash_handle, other._hash_handle);
}

//
// operations.
//

template <
  hash_algorithm_type HASH_ALGORITHM
>
void
hash<HASH_ALGORITHM>::update(
  const byte_buffer_ref input
  )
{
  BCryptHashData(
    _hash_handle,
    const_cast<PUCHAR>(input.get_buffer()),
    static_cast<ULONG>(input.get_size()),
    0);
}

template <
  hash_algorithm_type HASH_ALGORITHM
>
byte_buffer
hash<HASH_ALGORITHM>::compute(
  const byte_buffer_ref input
  )
{
  hash<HASH_ALGORITHM> md;
  md.update(input);
  return md.get();
}

//
// accessors.
//

template <
  hash_algorithm_type HASH_ALGORITHM
>
void
hash<HASH_ALGORITHM>::get(
  mutable_byte_buffer_ref output
  )
{
  mini_assert(output.get_size() >= hash_size_in_bytes);

  BCryptFinishHash(
    _hash_handle,
    output.get_buffer(),
    static_cast<ULONG>(hash_size_in_bytes),
    0);
}

template <
  hash_algorithm_type HASH_ALGORITHM
>
byte_buffer
hash<HASH_ALGORITHM>::get(
  void
  )
{
  byte_buffer result(hash_size_in_bytes);
  get(result);

  return result;
}

template <
  hash_algorithm_type HASH_ALGORITHM
>
void
hash<HASH_ALGORITHM>::destroy(
  void
  )
{
  if (_hash_handle)
  {
    BCryptDestroyHash(_hash_handle);
    _hash_handle = nullptr;
  }
}

//
// private methods.
//

template <
  hash_algorithm_type HASH_ALGORITHM
>
void
hash<HASH_ALGORITHM>::init(
  BCRYPT_ALG_HANDLE alg_handle
  )
{
  BCryptCreateHash(
    alg_handle,
    &_hash_handle,
    nullptr, // < pointer and size of the hash object.
    0,       // < since Win7 this parameter is optional.
    nullptr,
    0,
    0);
}

template <
  hash_algorithm_type HASH_ALGORITHM
>
void
hash<HASH_ALGORITHM>::init(
  BCRYPT_ALG_HANDLE alg_handle,
  const byte_buffer_ref key
  )
{
  BCryptCreateHash(
    alg_handle,
    &_hash_handle,
    nullptr,
    0,
    const_cast<PUCHAR>(key.get_buffer()),
    static_cast<ULONG>(key.get_size()),
    0);
}


template <
  hash_algorithm_type HASH_ALGORITHM
>
void
hash<HASH_ALGORITHM>::duplicate_internal(
  const hash& other
  )
{
  //
  // destroy previous hash.
  //
  destroy();

  BCryptDuplicateHash(
    other._hash_handle,
    &_hash_handle,
    nullptr, // < pointer and size of the hash object.
    0,       // < since Win7 this parameter is optional.
    0);
}

}
