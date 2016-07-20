#pragma once
#include <cstdint>
#include <initializer_list>
#include <vector>

#include <mini/allocator.h>
#include <mini/byte_buffer_ref.h>

namespace mini::collections {

template <
  typename T,
  typename ALLOCATOR_TYPE = allocator<T>
>
class list
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

    using allocator_type          = ALLOCATOR_TYPE;

    static const size_type not_found = (size_type)-1;

    //
    // constructors.
    //

    list(
      void
      );

    list(
      const list& other
      );

    list(
      list&& other
      );

    list(
      std::initializer_list<T> values
      );

    template <
      typename ITERATOR_TYPE
    >
    list(
      ITERATOR_TYPE begin,
      ITERATOR_TYPE end
      );

    list(
      size_type initial_size
      );

    list(
      const buffer_ref<T> buffer
      );

    //
    // destructor.
    //

    ~list(
      void
      );

    //
    // assign operators.
    //

    list&
    operator=(
      const list& other
      );

    list&
    operator=(
      list&& other
      );

    //
    // swap.
    //

    void
    swap(
      list& other
      );

    //
    // element access.
    //

    reference
    operator[](
      size_type index
      );

    const_reference
    operator[](
      size_type index
      ) const;

    reference
    at(
      size_type index
      );

    const_reference
    at(
      size_type index
      ) const;

    reference
    top(
      void
      );

    const_reference
    top(
      void
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

    bool
    is_empty(
      void
      ) const;

    size_type
    get_size(
      void
      ) const;

    void
    resize(
      size_type new_size,
      const_reference item = value_type()
      );

    size_type
    get_capacity(
      void
      ) const;

    void
    reserve(
      size_type new_capacity
      );

    void shrink(
      void
      );

    //
    // lookup.
    //

    size_type
    index_of(
      const value_type& item,
      size_type from_offset = 0
      ) const;

    bool
    contains(
      const value_type& item
      ) const;

    //
    // modifiers.
    //

    void
    add(
      const value_type& item
      );

    void
    add(
      value_type&& item
      );

    template <
      typename = std::enable_if_t<std::is_pod_v<T>>
    >
    void
    add_many(
      const buffer_ref<T> items
      );

    void
    push(
      const value_type& item
      );

    void
    push(
      value_type&& item
      );

    void
    insert(
      const value_type& item,
      size_type index
      );

    void
    insert(
      value_type&& item,
      size_type index
      );

    template <
      typename = std::enable_if_t<std::is_pod_v<T>>
    >
    void
    insert_many(
      const buffer_ref<T> items,
      size_type index
    );

    void
    remove(
      const value_type& item
      );

    void
    remove_at(
      size_type index
      );

    void
    remove_by_swap_at(
      size_type index
      );

    void
    pop(
      void
      );

    void clear(
      void
      );

    //
    // conversion operators.
    //

    template <
      typename = std::enable_if_t<!std::is_same_v<T, byte_type>>
    >
    operator buffer_ref<T>(
      void
      ) const;

    template <
      typename = std::enable_if_t<!std::is_same_v<T, byte_type>>
    >
    operator mutable_buffer_ref<T>(
      void
      );

    operator byte_buffer_ref(
      void
      ) const;

    operator mutable_byte_buffer_ref(
      void
      );

  private:
    void
    reserve_to_at_least(
      size_type desired_capacity
      );

    allocator_type _allocator;

    T* _first;
    T* _last;
    T* _end;
};

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
  mini::collections::list<T, ALLOCATOR_TYPE>& lhs,
  mini::collections::list<T, ALLOCATOR_TYPE>& rhs
  );

}

#include "list.inl"
