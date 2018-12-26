#pragma once
#include <mini/string.h>
#include <mini/byte_buffer_ref.h>

#include <iosfwd> // std::char_traits

namespace mini {

class string_ref
  : public buffer_ref<char>
{
  public:
    using traits_type = std::char_traits<char>;

    static constexpr auto not_found       = string::not_found;
    static constexpr auto zero_terminated = string::zero_terminated;

    using buffer_ref<char>::buffer_ref;

    //
    // constructors.
    //

    constexpr string_ref(
      void
      );

    constexpr string_ref(
      const string_ref& other
      ) = default;

    constexpr string_ref(
      const char* value
      );

    constexpr string_ref(
      const char* value, size_type count
      );

    //
    // assign operators.
    //

    constexpr string_ref&
    operator=(
      const string_ref& other
      ) = default;

    //
    // swap.
    //

    constexpr void
    swap(
      string_ref& other
      );

    //
    // capacity.
    //

    constexpr bool
    is_empty(
      void
      ) const;

    //
    // lookup.
    //

    inline size_type
    index_of(
      const string_ref item,
      size_type from_offset = 0
      ) const;

    inline size_type
    last_index_of(
      const string_ref item,
      size_type from_offset = 0
      ) const;

    inline bool
    contains(
      const string_ref item
      ) const;

    inline bool
    starts_with(
      const string_ref item
      ) const;

    inline bool
    ends_with(
      const string_ref item
      ) const;

    //
    // operations.
    //

    inline bool
    equals(
      const string_ref other
      ) const;

    inline int
    compare(
      const string_ref other
      ) const;

    inline string_ref
    substring(
      size_type offset
      ) const;

    inline string_ref
    substring(
      size_type offset,
      size_type length
      ) const;

    inline string_collection
    split(
      const string_ref delimiter,
      size_type count = size_type_max
      ) const;

#if !defined(MINI_MODE_KERNEL)

    inline int
    to_int(
      void
      ) const;

#endif

    inline bool
    is_zero_terminated(
      void
      ) const;

    //
    // conversion operators.
    //

    inline operator string(
      void
      ) const;

    inline operator byte_buffer_ref(
      void
      ) const;

    //
    // non-member operations.
    //

    inline friend bool
    operator==(
      const string_ref& lhs,
      const string& rhs
      );

    inline friend bool
    operator!=(
      const string_ref& lhs,
      const string& rhs
      );

    inline friend bool
    operator==(
      const string_ref& lhs,
      const string_ref& rhs
      );

    inline friend bool
    operator!=(
      const string_ref& lhs,
      const string_ref& rhs
      );

    inline friend string
    operator+(
        const string_ref& lhs,
        const string_ref& rhs
        );
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

#include "string_ref.inl"
