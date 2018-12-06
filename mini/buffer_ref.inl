#include "byte_buffer_ref.h"

#include <mini/memory.h>

namespace mini {

//
// constructors.
//

template <
  typename T
>
constexpr buffer_ref<T>::buffer_ref(
  void
  )
  : _begin(nullptr)
  , _end(nullptr)
{

}

template <
  typename T
>
constexpr buffer_ref<T>::buffer_ref(
  std::nullptr_t
  ) : buffer_ref<T>()
{

}

template <
  typename T
>
buffer_ref<T>::buffer_ref(
  std::initializer_list<T> values
  )
  : _begin((value_type*)values.begin())
  , _end((value_type*)values.end())
{

}

template <
  typename T
>
template <
  typename TConstIterator
>
constexpr buffer_ref<T>::buffer_ref(
  TConstIterator begin,
  TConstIterator end
  )
  : _begin((T*)begin)
  , _end((T*)end)
{

}

template <
  typename T
>
template <
  typename U,
  size_type N
>
constexpr buffer_ref<T>::buffer_ref(
  const U (&array)[N]
  )
  : _begin((T*)array)
  , _end((T*)array + N)
{

}

//
// assign operators.
//

template <
  typename T
>
buffer_ref<T>&
buffer_ref<T>::operator=(
  const buffer_ref& other
  )
{
  _begin = other._begin;
  _end = other._end;

  return *this;
}

template <
  typename T
>
buffer_ref<T>&
buffer_ref<T>::operator=(
  buffer_ref&& other
  )
{
  swap(other);

  return *this;
}

//
// swap.
//

template <
  typename T
>
void
buffer_ref<T>::swap(
  buffer_ref& other
  )
{
  mini::swap(_begin, other._begin);
  mini::swap(_end, other._end);
}

//
// element access.
//

template <
  typename T
>
constexpr typename buffer_ref<T>::const_reference
buffer_ref<T>::operator[](
  size_type index
  ) const
{
  return at(index);
}

template <
  typename T
>
constexpr typename buffer_ref<T>::const_reference
buffer_ref<T>::at(
  size_type index
  ) const
{
  return _begin[index];
}

template <
  typename T
>
const typename buffer_ref<T>::value_type*
buffer_ref<T>::get_buffer(
  void
  ) const
{
  return _begin;
}

//
// iterators.
//

template <
  typename T
>
typename buffer_ref<T>::const_iterator
buffer_ref<T>::begin(
  void
  ) const
{
  return _begin;
}

template <
  typename T
>
typename buffer_ref<T>::const_iterator
buffer_ref<T>::end(
  void
  ) const
{
  return _end;
}

//
// capacity.
//

template <
  typename T
>
bool
buffer_ref<T>::is_empty(
  void
  ) const
{
  return get_size() == 0;
}

template <
  typename T
>
typename buffer_ref<T>::size_type
buffer_ref<T>::get_size(
  void
  ) const
{
  return _end - _begin;
}

//
// pointer arithmetic.
//

template <
  typename T
>
typename buffer_ref<T>::const_pointer
buffer_ref<T>::operator++(
  void
  )
{
  return ++_begin;
}

template <
  typename T
>
typename buffer_ref<T>::const_pointer
buffer_ref<T>::operator++(
  int
  )
{
  return _begin++;
}

template <
  typename T
>
typename buffer_ref<T>::const_pointer
buffer_ref<T>::operator--(
  void
  )
{
  return --_begin;
}

template <
  typename T
>
typename buffer_ref<T>::const_pointer
buffer_ref<T>::operator--(
  int
  )
{
  return _begin--;
}

template <
  typename T
>
buffer_ref<T>
buffer_ref<T>::operator+(
  size_type rhs
  ) const
{
  buffer_ref<T> result = *this;
  result._begin += rhs;

  return result;
}

template <
  typename T
>
buffer_ref<T>
buffer_ref<T>::operator-(
  size_type rhs
  ) const
{
  buffer_ref<T> result = *this;
  result._begin -= rhs;

  return result;
}

//
// operations.
//

template <
  typename T
>
bool
buffer_ref<T>::equals(
  const buffer_ref other
  ) const
{
  return
    get_size() == other.get_size() &&
    compare(other) == 0;
}

template <
  typename T
>
int
buffer_ref<T>::compare(
  const buffer_ref other
  ) const
{
  return memory::compare(_begin, other._begin, get_size());
}

template <
  typename T
>
void
buffer_ref<T>::copy_to(
  mutable_buffer_ref<T> other,
  size_type size
  ) const
{
  size = size == (size_type)-1
    ? min(get_size(), other.get_size())
    : size;

  memory::copy(other._begin, _begin, size);
}

template <
  typename T
>
void
buffer_ref<T>::copy_to_unsafe(
  T* other
  ) const
{
  memory::copy(other, _begin, get_size());
}

template <
  typename T
>
void
buffer_ref<T>::reverse_copy_to(
  mutable_buffer_ref<T> other,
  size_type size
  ) const
{
  size = size == (size_type)-1
    ? min(get_size(), other.get_size())
    : size;

  memory::reverse_copy(other._begin, _begin, size);
}

template <
  typename T
>
buffer_ref<T>
buffer_ref<T>::slice(
  size_type begin,
  size_type end
  ) const
{
  end = end == (size_type)-1
    ? get_size()
    : end;

  return buffer_ref(_begin + begin, _begin + end);
}

//
// non-member operations.
//

template <
  typename T
>
bool
operator==(
  const buffer_ref<T>& lhs,
  const buffer_ref<T>& rhs
  )
{
  return lhs.equals(rhs);
}

template <
  typename T
>
void
swap(
  buffer_ref<T>& lhs,
  buffer_ref<T>& rhs
  )
{
  lhs.swap(rhs);
}

//
// constructors.
//

template <
  typename T
>
template <
  typename TIterator
>
mutable_buffer_ref<T>::mutable_buffer_ref(
  TIterator begin,
  TIterator end
  )
{
  this->_begin = (T*)begin;
  this->_end = (T*)end;
}

template <
  typename T
>
template <
  typename U,
  size_type N
>
mutable_buffer_ref<T>::mutable_buffer_ref(
  U (&array)[N]
  )
{
  this->_begin = (T*)array;
  this->_end = (T*)array + N;
}

//
// destructor.
//

template <
  typename T
>
mutable_buffer_ref<T>::~mutable_buffer_ref(
  void
  )
{

}

//
// assign operators.
//

template <
  typename T
>
mutable_buffer_ref<T>&
mutable_buffer_ref<T>::operator=(
  const mutable_buffer_ref& other
  )
{
  buffer_ref<T>::operator=(other);

  return *this;
}

template <
  typename T
>
mutable_buffer_ref<T>&
mutable_buffer_ref<T>::operator=(
  mutable_buffer_ref&& other
  )
{
  buffer_ref<T>::operator=(std::move(other));

  return *this;
}

//
// swap.
//

template <
  typename T
>
void
mutable_buffer_ref<T>::swap(
  mutable_buffer_ref& other
  )
{
  buffer_ref<T>::swap(other);
}

//
// element access.
//

template <
  typename T
>
typename mutable_buffer_ref<T>::reference
mutable_buffer_ref<T>::operator[](
  size_type index
  )
{
  return at(index);
}

template <
  typename T
>
typename mutable_buffer_ref<T>::reference
mutable_buffer_ref<T>::at(
  size_type index
  )
{
  return this->_begin[index];
}

template <
  typename T
>
typename mutable_buffer_ref<T>::value_type*
mutable_buffer_ref<T>::get_buffer(
  void
  )
{
  return this->_begin;
}

//
// iterators.
//

template <
  typename T
>
typename mutable_buffer_ref<T>::iterator
mutable_buffer_ref<T>::begin(
  void
  )
{
  return this->_begin;
}

template <
  typename T
>
typename mutable_buffer_ref<T>::iterator
mutable_buffer_ref<T>::end(
  void
  )
{
  return this->_end;
}

//
// pointer arithmetic.
//

template <
  typename T
>
typename mutable_buffer_ref<T>::pointer
mutable_buffer_ref<T>::operator++(
  void
  )
{
  return ++this->_begin;
}

template <
  typename T
>
typename mutable_buffer_ref<T>::pointer
mutable_buffer_ref<T>::operator++(
  int
  )
{
  return this->_begin++;
}

template <
  typename T
>
typename mutable_buffer_ref<T>::pointer
mutable_buffer_ref<T>::operator--(
  void
  )
{
  return --this->_begin;
}

template <
  typename T
>
typename mutable_buffer_ref<T>::pointer
mutable_buffer_ref<T>::operator--(
  int
  )
{
  return this->_begin--;
}


//
// operations.
//

template <
  typename T
>
mutable_buffer_ref<T>&
mutable_buffer_ref<T>::zero_buffer(
  void
  )
{
  memset(this->_begin, 0, this->get_size());
  return *this;
}

template <
  typename T
>
mutable_buffer_ref<T>&
mutable_buffer_ref<T>::copy_from(
  const buffer_ref<T> other
  )
{
  size_type size_to_copy = min(this->get_size(), other.get_size());
  memory::copy(this->_begin, other._begin, size_to_copy);
  return *this;
}

template <
  typename T
>
mutable_buffer_ref<T>&
mutable_buffer_ref<T>::reverse_copy_from(
  const buffer_ref<T> other
  )
{
  size_type size_to_copy = min(this->get_size(), other.get_size());
  memory::reverse_copy(this->_begin, other._begin, size_to_copy);
  return *this;
}

template <
  typename T
>
mutable_buffer_ref<T>
mutable_buffer_ref<T>::slice(
  size_type begin,
  size_type end
  )
{
  end = end == (size_type)-1
    ? this->get_size()
    : end;

  return mutable_buffer_ref<T>(this->_begin + begin, this->_begin + end);
}

}
