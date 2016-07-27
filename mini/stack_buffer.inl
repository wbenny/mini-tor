#include "stack_buffer.h"

namespace mini {

//
// swap.
//

template <
  typename T,
  size_t N
>
void
stack_buffer<T, N>::swap(
  stack_buffer& other
  )
{

}

//
// element access.
//

template <
  typename T,
  size_t N
>
typename stack_buffer<T, N>::reference
stack_buffer<T, N>::operator[](
  size_type index
  )
{
  return at(index);
}

template <
  typename T,
  size_t N
>
constexpr typename stack_buffer<T, N>::const_reference
stack_buffer<T, N>::operator[](
  size_type index
  ) const
{
  return at(index);
}

template <
  typename T,
  size_t N
>
typename stack_buffer<T, N>::reference
stack_buffer<T, N>::at(
  size_type index
  )
{
  return buffer[index];
}

template <
  typename T,
  size_t N
>
constexpr typename stack_buffer<T, N>::const_reference
stack_buffer<T, N>::at(
  size_type index
  ) const
{
  return buffer[index];
}

template <
  typename T,
  size_t N
>
typename stack_buffer<T, N>::value_type*
stack_buffer<T, N>::get_buffer(
  void
  )
{
  return buffer;
}

template <
  typename T,
  size_t N
>
const typename stack_buffer<T, N>::value_type*
stack_buffer<T, N>::get_buffer(
  void
  ) const
{
  return buffer;
}

//
// iterators.
//

template <
  typename T,
  size_t N
>
typename stack_buffer<T, N>::iterator
stack_buffer<T, N>::begin(
  void
  )
{
  return buffer;
}

template <
  typename T,
  size_t N
>
typename stack_buffer<T, N>::const_iterator
stack_buffer<T, N>::begin(
  void
  ) const
{
  return buffer;
}

template <
  typename T,
  size_t N
>
typename stack_buffer<T, N>::iterator
stack_buffer<T, N>::end(
  void
  )
{
  return buffer + N;
}

template <
  typename T,
  size_t N
>
typename stack_buffer<T, N>::const_iterator
stack_buffer<T, N>::end(
  void
  ) const
{
  return buffer + N;
}

//
// lookup.
//

template <
  typename T,
  size_t N
>
constexpr size_type
stack_buffer<T, N>::index_of(
  const T& item,
  size_type from_offset = 0
  ) const
{
  return
    from_offset >= N
    ? not_found
    : buffer[from_offset] == item
      ? from_offset
      : index_of(item, from_offset + 1);
}

//
// capacity.
//

template <
  typename T,
  size_t N
>
constexpr typename stack_buffer<T, N>::size_type
stack_buffer<T, N>::get_size(
  void
  ) const
{
  return N;
}

template <
  typename T,
  size_t N
>
stack_buffer<T, N>::operator buffer_ref<T>(
  void
  ) const
{
  return buffer_ref<T>(begin(), end());
}

template <
  typename T,
  size_t N
>
stack_buffer<T, N>::operator mutable_buffer_ref<T>(
  void
  )
{
  return mutable_buffer_ref<T>(begin(), end());
}

}
