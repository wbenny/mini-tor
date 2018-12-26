#pragma once
#include <mini/string.h>
#include <mini/byte_buffer_ref.h>

namespace mini {

class string_ref
  : public buffer_ref<char>
{
  public:
    static const size_type not_found       = string::not_found;
    static const size_type zero_terminated = string::zero_terminated;

    static const string_ref empty;

    using buffer_ref<char>::buffer_ref;

    //
    // constructors.
    //

    string_ref(
      void
      );

    string_ref(
      char value
      );

    template <
      size_type N
    >
    constexpr string_ref(
      const char (&value)[N]
      );

    string_ref(
      const char* value
      );

    string_ref(
      const string_ref& other
      ) = default;

    string_ref(
      string_ref&& other
      ) = default;


    //
    // assign operators.
    //

    string_ref&
    operator=(
      const string_ref& other
      );

    string_ref&
    operator=(
      string_ref&& other
      );

    //
    // swap.
    //

    void
    swap(
      string_ref& other
      );

    //
    // capacity.
    //

    bool
    is_empty(
      void
      ) const;

    //
    // lookup.
    //

    size_type
    index_of(
      const string_ref item,
      size_type from_offset = 0
      ) const;

    size_type
    last_index_of(
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
      const string_ref other
      ) const;

    int
    compare(
      const string_ref other
      ) const;

    string_ref
    substring(
      size_type offset
      ) const;

    string_ref
    substring(
      size_type offset,
      size_type length
      ) const;

    string_collection
    split(
      const string_ref delimiter,
      size_type count = size_type_max
      ) const;

#if !defined(MINI_MODE_KERNEL)

    int
    to_int(
      void
      ) const;

#endif

    bool
    is_zero_terminated(
      void
      ) const;

    //
    // conversion operators.
    //

    operator string(
      void
      ) const;

    operator byte_buffer_ref(
      void
      ) const;

    //
    // non-member operations.
    //

    friend bool
    operator==(
      const string_ref& lhs,
      const string& rhs
      );

    friend bool
    operator!=(
      const string_ref& lhs,
      const string& rhs
      );

    friend bool
    operator==(
      const string_ref& lhs,
      const string_ref& rhs
      );

    friend bool
    operator!=(
      const string_ref& lhs,
      const string_ref& rhs
      );

    friend string
    operator+(
        const string_ref& lhs,
        const string_ref& rhs
        );

  private:
    char _internal_char_buffer[2];
};

class mutable_string_ref
  : public mutable_buffer_ref<char>
{
  public:
    using mutable_buffer_ref<char>::mutable_buffer_ref;

    operator string() const
    {
      return string(this->begin(), this->get_size());
    }
};

template <>
struct hash<string_ref>
{
  size_type operator()(const string_ref value) const noexcept
  {
    return detail::hash_array_representation(value.get_buffer(), value.get_size());
  }
};

template <>
struct hash<mutable_string_ref>
{
  size_type operator()(const string_ref value) const noexcept
  {
    return detail::hash_array_representation(value.get_buffer(), value.get_size());
  }
};

}
