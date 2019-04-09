#pragma once
#include <mini/allocator.h>
#include <mini/byte_buffer_ref.h>

#include <initializer_list>

namespace mini::collections {

template <
  typename T,
  typename Allocator = allocator<T>
>
class list
{
  public:
    using value_type              = T;
    using size_type               = size_type;
    using difference_type         = pointer_difference_type;

    using allocator_type          = Allocator;

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
      typename TIterator
    >
    list(
      TIterator begin,
      TIterator end
      );

    list(
      size_type initial_size
      );

    list(
      const buffer_ref<T> buffer
      );

    template <
      size_type N
    >
    list(
      const T (&array)[N]
      );

    list(
      std::initializer_list<const buffer_ref<T>> items
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

    buffer_ref<T>
    slice(
      size_type begin,
      size_type end = (size_type)-1
      ) const;

    mutable_buffer_ref<T>
    slice(
      size_type begin,
      size_type end = (size_type)-1
      );

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

    void
    resize_unsafe(
      size_type new_size
      );

    size_type
    get_capacity(
      void
      ) const;

    void
    reserve(
      size_type new_capacity
      );

    void
    shrink(
      void
      );

    //
    // lookup.
    //

    bool
    equals(
      const list& other
      ) const;

    bool
    contains(
      const value_type& item
      ) const;

    size_type
    index_of(
      const value_type& item,
      size_type from_offset = 0
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

    void
    add_many(
      const buffer_ref<T> items
      );

    void
    add_many(
      std::initializer_list<const buffer_ref<T>> items
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

    void
    insert_many(
      const buffer_ref<T> items,
      size_type index
      );

    void
    remove(
      iterator it
      );

    void
    remove(
      const value_type& item
      );

    void
    remove_range(
      size_type from_offset,
      size_type count = (size_type)-1
      );

    void
    remove_at(
      size_type index
      );

    void
    remove_by_swap(
      iterator it
      );

    void
    remove_by_swap_at(
      size_type index
      );

    void
    pop(
      void
      );

    void
    clear(
      void
      );

    //
    // conversion operators.
    //

    operator buffer_ref<T>(
      void
      ) const;

    operator mutable_buffer_ref<T>(
      void
      );

    //
    // non-member operations.
    //

    friend bool
    operator==(
      const list<T, Allocator>& lhs,
      const list<T, Allocator>& rhs
      )
    {
      return lhs.equals(rhs);
    }

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
  typename Allocator
>
void
swap(
  mini::collections::list<T, Allocator>& lhs,
  mini::collections::list<T, Allocator>& rhs
  );

}

#include "list.inl"
