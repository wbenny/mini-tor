#include "list.h"

#include <mini/common.h>
#include <mini/memory.h>

namespace mini::collections {

//
// constructors.
//

template <
  typename T,
  typename ALLOCATOR_TYPE
>
list<T, ALLOCATOR_TYPE>::list(
  void
  )
  : _first(nullptr)
  , _last(nullptr)
  , _end(nullptr)
{

}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
list<T, ALLOCATOR_TYPE>::list(
  const list& other
  )
  : list<T, ALLOCATOR_TYPE>()
{
  reserve(other.get_capacity());

  for (auto&& e : other)
  {
    add(e);
  }
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
list<T, ALLOCATOR_TYPE>::list(
  list&& other
  )
  : list<T, ALLOCATOR_TYPE>()
{
  swap(other);
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
list<T, ALLOCATOR_TYPE>::list(
  std::initializer_list<T> values
)
  : list<T, ALLOCATOR_TYPE>()
{
  for (auto&& e : values)
  {
    add(e);
  }
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
template <
  typename ITERATOR_TYPE
>
list<T, ALLOCATOR_TYPE>::list(
  ITERATOR_TYPE begin,
  ITERATOR_TYPE end
  )
  : list<T, ALLOCATOR_TYPE>(end - begin)
{
  size_type index = 0;

  while (begin < end)
  {
    insert(*begin++, index++);
  }
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
list<T, ALLOCATOR_TYPE>::list(
  size_type initial_size
  )
  : list<T, ALLOCATOR_TYPE>()
{
  resize(initial_size);
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
list<T, ALLOCATOR_TYPE>::list(
  const buffer_ref<T> buffer
  )
  : list<T, ALLOCATOR_TYPE>(buffer.begin(), buffer.end())
{

}

//
// destructor.
//

template <
  typename T,
  typename ALLOCATOR_TYPE
>
list<T, ALLOCATOR_TYPE>::~list(
  void
  )
{
  _allocator.destroy_range(_first, _last);
  _allocator.deallocate(_first);
}

//
// assign operators.
//

template <
  typename T,
  typename ALLOCATOR_TYPE
>
list<T, ALLOCATOR_TYPE>&
list<T, ALLOCATOR_TYPE>::operator=(
  const list& other
  )
{
  reserve(other.get_capacity());

  for (auto&& e : other)
  {
    add(e);
  }

  return *this;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
list<T, ALLOCATOR_TYPE>&
list<T, ALLOCATOR_TYPE>::operator=(
  list&& other
  )
{
  swap(other);

  return *this;
}

//
// swap.
//

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::swap(
  list& other
  )
{
  mini::swap(_first, other._first);
  mini::swap(_last, other._last);
  mini::swap(_end, other._end);
}

//
// element access.
//

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::reference
list<T, ALLOCATOR_TYPE>::operator[](
  size_type index
  )
{
  return at(index);
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::const_reference
list<T, ALLOCATOR_TYPE>::operator[](
   size_type index
  ) const
{
  return at(index);
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::reference
list<T, ALLOCATOR_TYPE>::at(
  size_type index
  )
{
  return _first[index];
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::const_reference
list<T, ALLOCATOR_TYPE>::at(
  size_type index
  ) const
{
  return _first[index];
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::reference
list<T, ALLOCATOR_TYPE>::top(
  void
  )
{
  return _first[get_size() - 1];
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::const_reference
list<T, ALLOCATOR_TYPE>::top(
  void
  ) const
{
  return _first[get_size() - 1];
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::value_type*
list<T, ALLOCATOR_TYPE>::get_buffer(
  void
  )
{
  return _first;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
const typename list<T, ALLOCATOR_TYPE>::value_type*
list<T, ALLOCATOR_TYPE>::get_buffer(
  void
  ) const
{
  return _first;
}

//
// iterators.
//

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::iterator
list<T, ALLOCATOR_TYPE>::begin(
  void
  )
{
  return _first;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::const_iterator
list<T, ALLOCATOR_TYPE>::begin(
  void
  ) const
{
  return _first;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::iterator
list<T, ALLOCATOR_TYPE>::end(
  void
  )
{
  return _last;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::const_iterator
list<T, ALLOCATOR_TYPE>::end(
  void
  ) const
{
  return _last;
}

//
// capacity.
//

template <
  typename T,
  typename ALLOCATOR_TYPE
>
bool
list<T, ALLOCATOR_TYPE>::is_empty(
  void
  ) const
{
  return get_size() == 0;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::size_type
list<T, ALLOCATOR_TYPE>::get_size(
  void
  ) const
{
  return _last - _first;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::resize(
  size_type new_size,
  const_reference item
  )
{
  if (new_size < get_size())
  {
    _allocator.destroy_range(_first + new_size, _first + get_size());
  }
  else if (new_size <= get_capacity())
  {
    _allocator.construct_range(_first + get_size(), _first + new_size, item);
  }
  else if (new_size > get_capacity())
  {
    reserve(new_size);
    _allocator.construct_range(_first + get_size(), _first + new_size, item);
  }

  _last = _first + new_size;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::size_type
list<T, ALLOCATOR_TYPE>::get_capacity(
  void
  ) const
{
  return _end - _first;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::reserve(
  size_type new_capacity
  )
{
  if (new_capacity > get_capacity())
  {
    size_type old_size = get_size();
    T* new_first = _allocator.allocate(new_capacity);
    _allocator.move_range(_first, _last, new_first);
    _allocator.deallocate(_first);

    _first = new_first;
    _last = _first + old_size;
    _end = _first + new_capacity;
  }
}

//
// lookup.
//


template <
  typename T,
  typename ALLOCATOR_TYPE
>
typename list<T, ALLOCATOR_TYPE>::size_type
list<T, ALLOCATOR_TYPE>::index_of(
  const value_type& item,
  size_type from_offset
  ) const
{
  for (size_type i = from_offset; i < get_size(); i++)
  {
    if (_first[i] == item)
    {
      return i;
    }
  }

  return not_found;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
bool
list<T, ALLOCATOR_TYPE>::contains(
  const value_type& item
  ) const
{
  return index_of(item) != not_found;
}

//
// modifiers.
//

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::add(
  const value_type& item
  )
{
  insert(item, get_size());
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::add(
  value_type&& item
  )
{
  insert(std::move(item), get_size());
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
template<
  typename
>
void
list<T, ALLOCATOR_TYPE>::add_many(
  const buffer_ref<T> items
  )
{
  size_type new_size = get_size() + items.get_size();
  reserve_to_at_least(new_size);

  memcpy(&_first[get_size()], items.get_buffer(), items.get_size());

  //_allocator.construct_range(_first + get_size(), _first + new_size, T());

  _last = _first + new_size;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::push(
  const value_type& item
  )
{
  add(item);
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::push(
  value_type&& item
  )
{
  add(std::move(item));
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::insert(
  const value_type& item,
  size_type index
  )
{
  reserve_to_at_least(index + 1);

  _allocator.construct(_first + index, item);
  _last = _first + index + 1;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::insert(
  value_type&& item,
  size_type index
  )
{
  reserve_to_at_least(index + 1);

  _allocator.construct(_first + index, std::forward<value_type>(item));
  _last = _first + index + 1;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
template<
  typename
>
void
list<T, ALLOCATOR_TYPE>::insert_many(
  const buffer_ref<T> items,
  size_type index
  )
{
  size_type new_size = index + items.get_size();
  reserve_to_at_least(new_size);

  memcpy(&_first[index], items.get_buffer(), items.get_size());

  //_allocator.construct_range(_first + get_size(), _first + new_size, T());

  _last = _first + new_size;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::remove(
  const value_type& item
  )
{
  size_type index = index_of(item);

  if (index != not_found)
  {
    remove_at(index);
  }
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::remove_at(
  size_type index
  )
{
  if (index != (get_size() - 1))
  {
    do
    {
      _allocator.destroy(_first + index);
      _allocator.construct(_first + index, std::move(*(_first + index + 1)));
    } while (++index < (get_size() - 1));
  }

  //
  // destroy the last element
  //
  _allocator.destroy(_first + index);

  _last -= 1;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::remove_by_swap_at(
  size_type index
  )
{
  _allocator.destroy(_first + index);

  if (index != (get_size() - 1))
  {
    swap(_first[index], (_first[get_size() - 1]));
  }

  _last -= 1;
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::pop(
  void
  )
{
  remove_at(get_size() - 1);
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::clear(
  void
  )
{
  _last = _first;
}

//
// conversion operators.
//

template <
  typename T,
  typename ALLOCATOR_TYPE
>
template <
  typename
>
list<T, ALLOCATOR_TYPE>::operator buffer_ref<T>(
  void
  ) const
{
  return buffer_ref<T>(_first, _last);
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
template <
  typename
>
list<T, ALLOCATOR_TYPE>::operator mutable_buffer_ref<T>(
  void
  )
{
  return mutable_buffer_ref<T>(_first, _last);
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
list<T, ALLOCATOR_TYPE>::operator byte_buffer_ref(
  void
  ) const
{
  return byte_buffer_ref((byte_type*)_first, _last);
}

template <
  typename T,
  typename ALLOCATOR_TYPE
>
list<T, ALLOCATOR_TYPE>::operator mutable_byte_buffer_ref(
  void
  )
{
  return mutable_byte_buffer_ref((byte_type*)_first, _last);
}

//
// private methods.
//
template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
list<T, ALLOCATOR_TYPE>::reserve_to_at_least(
  size_type desired_capacity
  )
{
  if (desired_capacity > get_capacity())
  {
    size_type new_capacity = (get_capacity() + 1) << 2;

    while (desired_capacity >= new_capacity)
    {
      new_capacity <<= 2;
    }

    reserve(new_capacity);
  }
}

}

namespace mini {

//
// non-class functions.
//

template <
  typename T,
  typename ALLOCATOR_TYPE
>
void
swap(
  collections::list<T, ALLOCATOR_TYPE>& lhs,
  collections::list<T, ALLOCATOR_TYPE>& rhs
  )
{
  lhs.swap(rhs);
}

}
