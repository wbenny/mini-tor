#include "list.h"

#include <mini/common.h>
#include <mini/memory.h>

namespace mini::collections {

//
// constructors.
//

template <
  typename T,
  typename Allocator
>
list<T, Allocator>::list(
  void
  )
  : _first(nullptr)
  , _last(nullptr)
  , _end(nullptr)
{

}

template <
  typename T,
  typename Allocator
>
list<T, Allocator>::list(
  const list& other
  )
  : list<T, Allocator>()
{
  reserve(other.get_capacity());

  for (auto&& e : other)
  {
    add(e);
  }
}

template <
  typename T,
  typename Allocator
>
list<T, Allocator>::list(
  list&& other
  )
  : list<T, Allocator>()
{
  swap(other);
}

template <
  typename T,
  typename Allocator
>
list<T, Allocator>::list(
  std::initializer_list<T> values
  )
  : list<T, Allocator>()
{
  for (auto&& e : values)
  {
    add(e);
  }
}

template <
  typename T,
  typename Allocator
>
template <
  typename TIterator
>
list<T, Allocator>::list(
  TIterator begin,
  TIterator end
  )
  : list<T, Allocator>(end - begin)
{
  size_type index = 0;

  while (begin < end)
  {
    insert(*begin++, index++);
  }
}

template <
  typename T,
  typename Allocator
>
list<T, Allocator>::list(
  size_type initial_size
  )
  : list<T, Allocator>()
{
  resize(initial_size);
}

template <
  typename T,
  typename Allocator
>
list<T, Allocator>::list(
  const buffer_ref<T> buffer
  )
  : list<T, Allocator>(buffer.begin(), buffer.end())
{

}

template <
  typename T,
  typename Allocator
>
template <
  size_type N
>
list<T, Allocator>::list(
  const T(&array)[N]
  )
  : list<T, Allocator>(array, array + N)
{

}

template <
  typename T,
  typename Allocator
>
list<T, Allocator>::list(
  std::initializer_list<const buffer_ref<T>> items
  )
  : list<T, Allocator>()
{
  add_many(items);
}

//
// destructor.
//

template <
  typename T,
  typename Allocator
>
list<T, Allocator>::~list(
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
  typename Allocator
>
list<T, Allocator>&
list<T, Allocator>::operator=(
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
  typename Allocator
>
list<T, Allocator>&
list<T, Allocator>::operator=(
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
  typename Allocator
>
void
list<T, Allocator>::swap(
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
  typename Allocator
>
typename list<T, Allocator>::reference
list<T, Allocator>::operator[](
  size_type index
  )
{
  return at(index);
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::const_reference
list<T, Allocator>::operator[](
   size_type index
  ) const
{
  return at(index);
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::reference
list<T, Allocator>::at(
  size_type index
  )
{
  return _first[index];
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::const_reference
list<T, Allocator>::at(
  size_type index
  ) const
{
  return _first[index];
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::reference
list<T, Allocator>::top(
  void
  )
{
  return _first[get_size() - 1];
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::const_reference
list<T, Allocator>::top(
  void
  ) const
{
  return _first[get_size() - 1];
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::value_type*
list<T, Allocator>::get_buffer(
  void
  )
{
  return _first;
}

template <
  typename T,
  typename Allocator
>
const typename list<T, Allocator>::value_type*
list<T, Allocator>::get_buffer(
  void
  ) const
{
  return _first;
}

template <
  typename T,
  typename Allocator
>
buffer_ref<T>
list<T, Allocator>::slice(
  size_type begin,
  size_type end
  ) const
{
  end = end == (size_type)-1
    ? get_size()
    : end;

  return buffer_ref<T>(_first + begin, _first + end);
}

template <
  typename T,
  typename Allocator
>
mutable_buffer_ref<T>
list<T, Allocator>::slice(
  size_type begin,
  size_type end
  )
{
  end = end == (size_type)-1
    ? get_size()
    : end;

  return mutable_buffer_ref<T>(_first + begin, _first + end);
}

//
// iterators.
//

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::iterator
list<T, Allocator>::begin(
  void
  )
{
  return _first;
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::const_iterator
list<T, Allocator>::begin(
  void
  ) const
{
  return _first;
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::iterator
list<T, Allocator>::end(
  void
  )
{
  return _last;
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::const_iterator
list<T, Allocator>::end(
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
  typename Allocator
>
bool
list<T, Allocator>::is_empty(
  void
  ) const
{
  return get_size() == 0;
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::size_type
list<T, Allocator>::get_size(
  void
  ) const
{
  return _last - _first;
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::resize(
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
    reserve(max(new_size, (get_size() * 3) / 2));
    _allocator.construct_range(_first + get_size(), _first + new_size, item);
  }

  _last = _first + new_size;
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::resize_unsafe(
  size_type new_size
  )
{
  if (new_size > get_capacity())
  {
    reserve(max(new_size, (get_size() * 3) / 2));
  }

  _last = _first + new_size;
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::size_type
list<T, Allocator>::get_capacity(
  void
  ) const
{
  return _end - _first;
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::reserve(
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
  typename Allocator
>
bool
list<T, Allocator>::equals(
  const list& other
  ) const
{
  return buffer_ref<T>(*this).equals(other);
}

template <
  typename T,
  typename Allocator
>
bool
list<T, Allocator>::contains(
  const value_type& item
  ) const
{
  return index_of(item) != not_found;
}

template <
  typename T,
  typename Allocator
>
typename list<T, Allocator>::size_type
list<T, Allocator>::index_of(
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

//
// modifiers.
//

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::add(
  const value_type& item
  )
{
  insert(item, get_size());
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::add(
  value_type&& item
  )
{
  insert(std::move(item), get_size());
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::add_many(
  const buffer_ref<T> items
  )
{
  size_type new_size = get_size() + items.get_size();
  reserve_to_at_least(new_size);

  //
  // TODO:
  // fix for non-POD types.
  //
  memory::copy(&_first[get_size()], items.get_buffer(), items.get_size() * sizeof(T));

  _last = _first + new_size;
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::add_many(
  std::initializer_list<const buffer_ref<T>> items
  )
{
  for (auto&& e : items)
  {
    add_many(e);
  }
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::push(
  const value_type& item
  )
{
  add(item);
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::push(
  value_type&& item
  )
{
  add(std::move(item));
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::insert(
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
  typename Allocator
>
void
list<T, Allocator>::insert(
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
  typename Allocator
>
void
list<T, Allocator>::insert_many(
  const buffer_ref<T> items,
  size_type index
  )
{
  size_type new_size = index + items.get_size();
  reserve_to_at_least(new_size);

  //
  // TODO:
  // fix for non-POD types.
  //
  memory::copy(&_first[index], items.get_buffer(), items.get_size());

  _last = _first + new_size;
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::remove(
  iterator it
  )
{
  remove_at(it - begin());
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::remove(
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
  typename Allocator
>
void
list<T, Allocator>::remove_range(
  size_type from_offset,
  size_type count
  )
{
  if (is_empty())
  {
    return;
  }

  const size_type max_count = get_size() - 1;

  if (count == (size_type)-1)
  {
    count = max_count;
  }

  //
  // special case #1
  // removing all items
  //
  if (from_offset == 0 && count == max_count)
  {
    clear();
  }
  //
  // special case #2
  // removing items from any location to the end
  //
  else if (count == max_count)
  {
    resize_unsafe(get_size() - count);
  }
  //
  // everything else:
  // move buffer & resize
  //
  else
  {
    memory::move(
      _first + from_offset,
      _first + from_offset + count,
      get_size() - count - from_offset);

    resize_unsafe(get_size() - count);
  }
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::remove_at(
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
  typename Allocator
>
void
list<T, Allocator>::remove_by_swap(
  iterator it
  )
{
  remove_by_swap_at(it - begin());
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::remove_by_swap_at(
  size_type index
  )
{
  _allocator.destroy(_first + index);

  if (index != (get_size() - 1))
  {
    ::mini::swap(_first[index], (_first[get_size() - 1]));
  }

  _last -= 1;
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::pop(
  void
  )
{
  remove_at(get_size() - 1);
}

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::clear(
  void
  )
{
  _allocator.destroy_range(_first, _last);
  _last = _first;
}

//
// conversion operators.
//

template <
  typename T,
  typename Allocator
>
list<T, Allocator>::operator buffer_ref<T>(
  void
  ) const
{
  return buffer_ref<T>(_first, _last);
}

template <
  typename T,
  typename Allocator
>
list<T, Allocator>::operator mutable_buffer_ref<T>(
  void
  )
{
  return mutable_buffer_ref<T>(_first, _last);
}

//
// private methods.
//

template <
  typename T,
  typename Allocator
>
void
list<T, Allocator>::reserve_to_at_least(
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
  typename Allocator
>
void
swap(
  collections::list<T, Allocator>& lhs,
  collections::list<T, Allocator>& rhs
  )
{
  lhs.swap(rhs);
}

}
