#pragma once
#include <mini/common.h>

#include <type_traits>

namespace mini {

template <
  typename T
>
class ptr
{
  public:
    using element_type    = T;

    using pointer         = T*;
    using const_pointer   = const T*;

    using reference       = T&;
    using const_reference = const T&;

    //
    // constructors.
    //

    ptr(
      void
      ) = default;

    ptr(
      ptr<T>&& other
      );

    /*explicit*/ ptr(
      pointer p
      );

    //
    // destructor.
    //

    ~ptr(
      void
      );

    //
    // assign operators.
    //

    // ptr<T>&
    // operator=(
    //   pointer p
    //   );

    ptr<T>&
    operator=(
      ptr<T>&& other
      );

    //
    // pointer arithmetic.
    //

    T*
    operator+(
      size_type rhs
      );

    T*
    operator-(
      size_type rhs
      );

    //
    // swap.
    //

    void
    swap(
      ptr<T>& other
      );

    //
    // modifiers.
    //

    pointer
    release(
      void
      );

    void
    reset(
      pointer p = nullptr
      );

    //
    // observers.
    //

    pointer
    get(
      void
      );

    const_pointer
    get(
      void
      ) const;

    explicit operator bool(
      void
      ) const;

    std::add_lvalue_reference_t<T>
    operator*(
      void
      ) const;

    pointer
    operator->(
      void
      ) const;

    T&
    operator[](
      size_type index
      ) const;

    // friend bool
    // operator==(
    //   const ptr& lhs,
    //   std::nullptr_t
    //   );
    //
    // friend bool
    // operator!=(
    //   const ptr& lhs,
    //   std::nullptr_t
    //   );

  private:
    pointer _raw_pointer = nullptr;
};

template <
  typename T,
  typename... Args
>
ptr<T>
make_ptr(Args&&... args)
{
  return ptr<T>(new T(std::forward<Args>(args)...));
}

template <
  typename T
>
void
swap(
  ptr<T>& lhs,
  ptr<T>& rhs
  );

}

#include "ptr.inl"
