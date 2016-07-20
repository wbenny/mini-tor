#pragma once
#include <mini/common.h>
#include <mini/byte_buffer_ref.h>

namespace mini {

template <
  typename T,
  size_t N
>
class stack_buffer
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

    static constexpr size_type not_found = (size_type)-1;

    //
    // constructors.
    //

//     stack_buffer(
//       void
//       );
//
//     stack_buffer(
//       const stack_buffer& other
//       );
//
//     stack_buffer(
//       stack_buffer&& other
//       );
//
//     stack_buffer(
//       std::initializer_list<T> values
//       );
//
//     template <
//       typename ITERATOR_TYPE
//     >
//     stack_buffer(
//       ITERATOR_TYPE begin,
//       ITERATOR_TYPE end
//       );
//
//     stack_buffer(
//       size_type initial_size
//       );

    //
    // destructor.
    //

//     ~stack_buffer(
//       void
//       );

    //
    // assign operators.
    //

//     stack_buffer&
//     operator=(
//       const stack_buffer& other
//       );
//
//     stack_buffer&
//     operator=(
//       stack_buffer&& other
//       );

    //
    // swap.
    //

    void
    swap(
      stack_buffer& other
      );

    //
    // element access.
    //

    reference
    operator[](
      size_type index
      );

    constexpr const_reference
    operator[](
      size_type index
      ) const;

    reference
    at(
      size_type index
      );

    constexpr const_reference
    at(
      size_type index
      ) const;

    value_type*
    get_buffer(
      void
      );

    const value_type*
    get_buffer(
      void
      ) const;

    //
    // iterators.
    //

    iterator
    begin(
      void
      );

    const_iterator
    begin(
      void
      ) const;

    iterator
    end(
      void
      );

    const_iterator
    end(
      void
      ) const;

    //
    // capacity.
    //

    constexpr size_type
    get_size(
      void
      ) const;

    operator buffer_ref<T>(
      void
      ) const;

    operator mutable_buffer_ref<T>(
      void
      );

    //
    // lookup.
    //

    constexpr size_type
    index_of(
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
    // public buffer.
    //
    T buffer[N];
};

template <
  size_t N
>
using stack_byte_buffer = stack_buffer<byte_type, N>;

}

#include "stack_buffer.inl"
