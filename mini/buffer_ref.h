#pragma once
#include <mini/common.h>

#include <cstring>
#include <type_traits>

namespace mini {

template <
  typename T
>
class buffer_ref;

template <
  typename T
>
class mutable_buffer_ref;

template <
  typename T
>
class buffer_ref
{
  public:
    using value_type              = T;
    using size_type               = size_type;
    using pointer_difference_type = pointer_difference_type;

    using pointer                 = value_type*;
    using const_pointer           = const value_type*;

    using reference               = value_type&;
    using const_reference         = const value_type&;

    using iterator                = pointer;
    using const_iterator          = const_pointer;

    //
    // constructors.
    //

    constexpr buffer_ref(
      void
      );

    constexpr buffer_ref(
      const buffer_ref& other
      ) = default;

    constexpr buffer_ref(
      buffer_ref&& other
      ) = default;

    constexpr buffer_ref(
      std::nullptr_t
      );

    buffer_ref(
      std::initializer_list<T> values
      );

    template <
      typename CONST_ITERATOR_TYPE
    >
    constexpr buffer_ref(
      CONST_ITERATOR_TYPE begin,
      CONST_ITERATOR_TYPE end
      );

    template <
      typename U,
      size_type N
    >
    constexpr buffer_ref(
      const U (&array)[N]
      );

    //
    // destructor.
    //

    ~buffer_ref(
      void
      ) = default;

    //
    // assign operators.
    //

    buffer_ref&
    operator=(
      const buffer_ref& other
      );

    buffer_ref&
    operator=(
      buffer_ref&& other
      );

    //
    // swap.
    //

    void
    swap(
      buffer_ref& other
      );

    //
    // element access.
    //

    constexpr const_reference
    operator[](
      size_type index
      ) const;

    constexpr const_reference
    at(
      size_type index
      ) const;

    const value_type*
    get_buffer(
      void
      ) const;

    //
    // iterators.
    //

    const_iterator
    begin(
      void
      ) const;

    const_iterator
    end(
      void
      ) const;

    //
    // capacity.
    //

    bool
    is_empty(
      void
      ) const;

    size_type
    get_size(
      void
      ) const;

    //
    // pointer arithmetic.
    //

    const_pointer
    operator++(
      void
      );

    const_pointer
    operator++(
      int
      );

    const_pointer
    operator--(
      void
      );

    const_pointer
    operator--(
      int
      );

    buffer_ref<T>
    operator+(
      size_t rhs
      ) const;

    buffer_ref<T>
    operator-(
      size_t rhs
      ) const;

    //
    // operations.
    //

    bool
    equals(
      const buffer_ref other
      ) const;

    int
    compare(
      const buffer_ref other
      ) const;

    void
    copy_to(
      mutable_buffer_ref<T> other,
      size_type size = (size_type)-1
      ) const;

    buffer_ref
    slice(
      size_type begin,
      size_type end = (size_type)-1
      ) const;

    //
    // non-member operations.
    //

    friend bool
    operator==(
      const buffer_ref<T>& lhs,
      const buffer_ref<T>& rhs
      );

  protected:
    template <
      typename T
    >
    friend class mutable_buffer_ref;

    T* _begin;
    T* _end;
};

template <
  typename T
>
class mutable_buffer_ref
  : public buffer_ref<T>
{
  public:
    using value_type              = T;
    using size_type               = size_type;
    using pointer_difference_type = pointer_difference_type;

    using pointer                 = value_type*;
    using const_pointer           = const value_type*;

    using reference               = value_type&;
    using const_reference         = const value_type&;

    using iterator                = pointer;
    using const_iterator          = const_pointer;

    //
    // constructors.
    //

    mutable_buffer_ref(
      void
      ) = default;

    mutable_buffer_ref(
      const mutable_buffer_ref& other
      ) = default;

    mutable_buffer_ref(
      mutable_buffer_ref&& other
      ) = default;

    template <
      typename ITERATOR_TYPE
    >
    mutable_buffer_ref(
      ITERATOR_TYPE begin,
      ITERATOR_TYPE end
      );

    template <
      typename U,
      size_type N
    >
    mutable_buffer_ref(
      U (&array)[N]
      );

    //
    // destructor.
    //

    ~mutable_buffer_ref(
      void
      );

    //
    // assign operators.
    //

    mutable_buffer_ref&
    operator=(
      const mutable_buffer_ref& other
      );

    mutable_buffer_ref&
    operator=(
      mutable_buffer_ref&& other
      );

    //
    // swap.
    //

    void
    swap(
      mutable_buffer_ref& other
      );

    //
    // element access.
    //

    using buffer_ref<T>::operator[];
    using buffer_ref<T>::at;
    using buffer_ref<T>::get_buffer;

    reference
    operator[](
      size_type index
      );

    reference
    at(
      size_type index
      );

    value_type*
    get_buffer(
      void
      );

    //
    // iterators.
    //

    using buffer_ref<T>::begin;
    using buffer_ref<T>::end;

    iterator
    begin(
      void
      );

    iterator
    end(
      void
      );

    //
    // pointer arithmetic.
    //

    using buffer_ref<T>::operator++;
    using buffer_ref<T>::operator--;

    pointer
    operator++(
      void
      );

    pointer
    operator++(
      int
      );

    pointer
    operator--(
      void
      );

    pointer
    operator--(
      int
      );

    //
    // operations.
    //

    mutable_buffer_ref&
    zero_buffer(
      void
      );

    mutable_buffer_ref&
    copy_from(
      const buffer_ref other
      );

    mutable_buffer_ref
    slice(
      size_type begin,
      size_type end = (size_type)-1
      );
};

template <
  typename T
>
void
swap(
  buffer_ref<T>& lhs,
  buffer_ref<T>& rhs
  );

}

#include "buffer_ref.inl"
