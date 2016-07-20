#include "stack_buffer.h"

namespace mini {

// template <
//   typename T,
//   size_t N
// >
// stack_buffer<T, N>::stack_buffer(
//   void
//   )
// {
//
// }
//
// template <
//   typename T,
//   size_t N
// >
// stack_buffer<T, N>::stack_buffer(
//   const stack_buffer& other
//   )
// {
//
// }
//
// template <
//   typename T,
//   size_t N
// >
// stack_buffer<T, N>::stack_buffer(
//   stack_buffer&& other
//   )
// {
//
// }
//
// template <
//   typename T,
//   size_t N
// >
// stack_buffer<T, N>::stack_buffer(
//   std::initializer_list<T> values
//   )
// {
//
// }
//
// template <
//   typename T,
//   size_t N
// >
// template <
//   typename ITERATOR_TYPE
// >
// stack_buffer<T, N>::stack_buffer(
//   ITERATOR_TYPE begin,
//   ITERATOR_TYPE end
//   )
// {
//
// }
//
// template <
//   typename T,
//   size_t N
// >
// stack_buffer<T, N>::stack_buffer(
//   size_type initial_size
//   )
// {
//
// }
//
// //
// // destructor.
// //
//
// template <
//   typename T,
//   size_t N
// >
// stack_buffer<T, N>::~stack_buffer(
//   void
//   )
// {
//
// }

//
// assign operators.
//

// template <
//   typename T,
//   size_t N
// >
// stack_buffer<T, N>&
// stack_buffer<T, N>::operator=(
//   const stack_buffer& other
//   )
// {
//
// }
//
// template <
//   typename T,
//   size_t N
// >
// stack_buffer<T, N>&
// stack_buffer<T, N>::operator=(
//   stack_buffer&& other
//   )
// {
//
// }

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
