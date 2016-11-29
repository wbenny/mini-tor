#pragma once
#include <mini/common.h>
#include <mini/collections/list.h>
#include <mini/byte_buffer_ref.h>

#include <type_traits>

namespace mini {

class string;
class string_ref;
class mutable_string_ref;
class string_hash;

using string_collection = collections::list<string>;

class string
{
  public:
    using value_type              = char;
    using size_type               = size_type;
    using pointer_difference_type = pointer_difference_type;

    using pointer                 = value_type*;
    using const_pointer           = const value_type*;

    using reference               = value_type&;
    using const_reference         = const value_type&;

    using iterator                = pointer;
    using const_iterator          = const_pointer;

    static const size_type not_found       = (size_type)-1;
    static const size_type zero_terminated = (size_type)-1;

    //
    // constructors.
    //

    string(
      void
      );

    string(
      const string& other
      );

    string(
      string&& other
      );

    string(
      size_type initial_size
      );

    string(
      const value_type* other,
      size_type size = zero_terminated
      );

    //
    // destructor.
    //

    ~string(
      void
      );

    //
    // assign operators.
    //

    string&
    operator=(
      const string& other
      );

    string&
    operator=(
      string&& other
      );

    string&
    operator+=(
      const string& other
      );

    //
    // swap.
    //

    void
    swap(
      string& other
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

    value_type&
    at(
      size_type index
      );

    const value_type&
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
      value_type item = '\0'
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

    size_type
    index_of(
      const string_ref item,
      size_type from_offset = 0
      ) const;

    bool
    contains(
      const string_ref item
      ) const;

    bool
    starts_with(
      const string_ref item
      ) const;

    bool
    ends_with(
      const string_ref item
      ) const;

    //
    // operations.
    //

    bool
    equals(
      const string& other
      ) const;

    int
    compare(
      const string& other
      ) const;

    string
    substring(
      size_type offset
      ) const;

    string
    substring(
      size_type offset,
      size_type length
      ) const;

    string_collection
    split(
      const string_ref delimiter,
      size_type count = size_type_max
      ) const;

    void
    from_int(
      int value
      );

    int
    to_int(
      void
      ) const;

    //
    // modifiers.
    //

    string&
    assign(
      const value_type* other,
      size_type size = zero_terminated
      );

    string&
    assign(
      const string& other
      );

    string&
    assign(
      string&& other
      );

    string&
    append(
      value_type other
      );

    string&
    append(
      const value_type* other,
      size_type size = zero_terminated
      );

    string&
    append(
      const string& other
      );

    void
    clear(
      void
      );

    //
    // static methods
    //

    static string
    format(
      const string_ref format,
      ...
      );

    //
    // conversion operators.
    //

    operator byte_buffer_ref(
      void
      ) const;

    operator mutable_byte_buffer_ref(
      void
      );

    operator string_ref(
      void
      ) const;

    operator mutable_string_ref(
      void
      );

    operator string_hash(
      void
      ) const;

    //
    // non-member operations.
    //

    friend bool
    operator==(
      const string& lhs,
      const string& rhs
      );

    friend bool
    operator!=(
      const string& lhs,
      const string& rhs
      );

    friend string
    operator+(
      const string& lhs,
      const string& rhs
      );

  private:
    collections::list<value_type> _buffer;
};

void
swap(
  string& lhs,
  string& rhs
  );

}

#include <mini/string_ref.h>

