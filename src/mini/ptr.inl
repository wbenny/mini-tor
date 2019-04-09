#include "ptr.h"

#include "common.h"

namespace mini {

//
// constructors.
//

template <
  typename T
>
ptr<T>::ptr(
  ptr<T>&& other
  )
{
  swap(other);
}

template <
  typename T
>
ptr<T>::ptr(
  pointer p
  )
  : _raw_pointer(p)
{

}

//
// destructor.
//

template <
  typename T
>
ptr<T>::~ptr(
  void
  )
{
  reset();
}

//
// assign operators.
//

// template <
//   typename T
// >
// ptr<T>&
// ptr<T>::operator=(
//   pointer p
//   )
// {
//   reset(p);
//
//   return *this;
// }

template <
  typename T
>
ptr<T>&
ptr<T>::operator=(
  ptr<T>&& other
  )
{
  swap(other);

  return *this;
}

//
// pointer arithmetic.
//

template <
  typename T
>
T*
ptr<T>::operator+(
  size_type rhs
  )
{
  return _raw_pointer + rhs;
}

template <
  typename T
>
T*
ptr<T>::operator-(
  size_type rhs
  )
{
  return _raw_pointer - rhs;
}

//
// swap.
//

template <
  typename T
>
void
ptr<T>::swap(
  ptr<T>& other
  )
{
  mini::swap(_raw_pointer, other._raw_pointer);
}

//
// modifiers.
//

template <
  typename T
>
typename ptr<T>::pointer
ptr<T>::release(
  void
  )
{
  pointer result = _raw_pointer;
  _raw_pointer = nullptr;

  return result;
}

template <
  typename T
>
void
ptr<T>::reset(
  pointer p
  )
{
  if (_raw_pointer && p != _raw_pointer)
  {
    delete _raw_pointer;
  }

  _raw_pointer = p;
}

//
// observers.
//

template <
  typename T
>
typename ptr<T>::pointer
ptr<T>::get(
  void
  )
{
  return _raw_pointer;
}

template <
  typename T
>
typename ptr<T>::const_pointer
ptr<T>::get(
  void
  ) const
{
  return _raw_pointer;
}

template <
  typename T
>
ptr<T>::operator bool(
  void
  ) const
{
  return !!_raw_pointer;
}

template <
  typename T
>
std::add_lvalue_reference_t<T>
ptr<T>::operator*(
  void
  ) const
{
  return *_raw_pointer;
}

template <
  typename T
>
typename ptr<T>::pointer
ptr<T>::operator->(
  void
  ) const
{
  return _raw_pointer;
}

template <
  typename T
>
T&
ptr<T>::operator[](
  size_type index
  ) const
{
  return _raw_pointer[index];
}

//
// non-member functions.
//

template <
  typename T
>
bool
operator==(
  const ptr<T>& lhs,
  std::nullptr_t
  )
{
  return lhs.get() == nullptr;
}

template <
  typename T
>
bool
operator!=(
  const ptr<T>& lhs,
  std::nullptr_t
  )
{
  return !(lhs == nullptr);
}

template <
  typename T
>
void
swap(
  ptr<T>& lhs,
  ptr<T>& rhs
  )
{
  lhs.swap(rhs);
}

}
