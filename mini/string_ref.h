#pragma once
#include <mini/string.h>
#include <mini/byte_buffer_ref.h>

namespace mini {

class string_ref
  : public buffer_ref<char>
{
  public:
    using buffer_ref<char>::buffer_ref;

    string_ref(
      char value
      );

    template <
      size_t N
    >
    constexpr string_ref(
      const char (&value)[N]
      );

    string_ref(
      const char* value
      );

    size_type
    get_size(
      void
      ) const;

    operator string(
      void
      ) const;

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
      return string(begin(), get_size());
    }
};

}
