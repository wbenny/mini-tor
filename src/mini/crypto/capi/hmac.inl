#include "hmac.h"
#include "provider.h"

namespace mini::crypto::capi {

//
// constructors.
//

template <
  typename HASH_TYPE
>
hmac<HASH_TYPE>::hmac(
  const byte_buffer_ref key
  )
  : HASH_TYPE(detail::hash_algorithm_type_map[static_cast<int>(hash_algorithm)], key, _key_handle)
{

}

template <
  typename HASH_TYPE
>
hmac<HASH_TYPE>::hmac(
  const hmac& other
  )
{
  duplicate_internal(other);
}

template <
  typename HASH_TYPE
>
hmac<HASH_TYPE>::hmac(
  hmac&& other
  )
{
  swap(other);
}

//
// destructor.
//

template <
  typename HASH_TYPE
>
hmac<HASH_TYPE>::~hmac(
  void
  )
{
  destroy();
}

//
// assign operators.
//

template <
  typename HASH_TYPE
>
hmac<HASH_TYPE>&
hmac<HASH_TYPE>::operator=(
  const hmac& other
  )
{
  duplicate_internal(other);
  return *this;
}

template <
  typename HASH_TYPE
>
hmac<HASH_TYPE>&
hmac<HASH_TYPE>::operator=(
  hmac&& other
  )
{
  swap(other);
  return *this;
}

template <
  typename HASH_TYPE
>
hmac<HASH_TYPE>
hmac<HASH_TYPE>::duplicate(
  void
  )
{
  return hmac<HASH_TYPE>(*this);
}

//
// swap.
//

template <
  typename HASH_TYPE
>
void
hmac<HASH_TYPE>::swap(
  hmac& other
  )
{
  HASH_TYPE::swap(other);
  mini::swap(_key_handle, other._key_handle);
}

//
// operations.
//

template <
  typename HASH_TYPE
>
byte_buffer
hmac<HASH_TYPE>::compute(
  const byte_buffer_ref key,
  const byte_buffer_ref input
  )
{
  hmac<HASH_TYPE> md(key);
  md.update(input);
  return md.get();
}

template <
  typename HASH_TYPE
>
void
hmac<HASH_TYPE>::destroy(
  void
  )
{
  HASH_TYPE::destroy();

  if (_key_handle)
  {
    CryptDestroyKey(_key_handle);
    _key_handle = 0;
  }
}


}
