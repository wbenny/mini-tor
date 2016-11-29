#include "hash_template_base.h"

namespace mini::crypto {

template <
  ALG_ID ALGORITHM_ID,
  size_type HASH_SIZE
>
hash_template_base<ALGORITHM_ID, HASH_SIZE>::hash_template_base(
  void
  )
  : hash_base(ALGORITHM_ID)
{

}

template <
  ALG_ID ALGORITHM_ID,
  size_type HASH_SIZE
>
hash_template_base<ALGORITHM_ID, HASH_SIZE>::hash_template_base(
  const hash_template_base& other
  )
  : hash_base(other)
{

}

template <
  ALG_ID ALGORITHM_ID,
  size_type HASH_SIZE
>
hash_template_base<ALGORITHM_ID, HASH_SIZE>::hash_template_base(
  hash_template_base&& other
  )
  : hash_base(std::move(other))
{

}

template <
  ALG_ID ALGORITHM_ID,
  size_type HASH_SIZE
>
hash_template_base<ALGORITHM_ID, HASH_SIZE>&
hash_template_base<ALGORITHM_ID, HASH_SIZE>::operator=(
  const hash_template_base& other
  )
{
  return static_cast<hash_template_base&>(hash_base::operator=(other));
}

template <
  ALG_ID ALGORITHM_ID,
  size_type HASH_SIZE
>
hash_template_base<ALGORITHM_ID, HASH_SIZE>&
hash_template_base<ALGORITHM_ID, HASH_SIZE>::operator=(
  hash_template_base&& other
  )
{
  return static_cast<hash_template_base&>(hash_base::operator=(std::move(other)));
}

template <
  ALG_ID ALGORITHM_ID,
  size_type HASH_SIZE
>
hash_template_base<ALGORITHM_ID, HASH_SIZE>
hash_template_base<ALGORITHM_ID, HASH_SIZE>::duplicate(
  void
  )
{
  return hash_template_base(*this);
}

template <
  ALG_ID ALGORITHM_ID,
  size_type HASH_SIZE
>
size_type
hash_template_base<ALGORITHM_ID, HASH_SIZE>::get_hash_size(
  void
  ) const
{
  return hash_size_in_bytes;
}

template <
  ALG_ID ALGORITHM_ID,
  size_type HASH_SIZE
>
byte_buffer
hash_template_base<ALGORITHM_ID, HASH_SIZE>::hash(
  const byte_buffer_ref input
  )
{
  hash_template_base md;
  md.update(input);
  return md.get();
}

}