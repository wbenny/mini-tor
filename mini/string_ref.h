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
      )
      : buffer_ref<char>(_internal_char_buffer, _internal_char_buffer + 2)
    {
      _internal_char_buffer[0] = value;
      _internal_char_buffer[1] = '\0';
    }

    string_ref(
      const char* value
      )
      : buffer_ref<char>(value, value + strlen(value) + 1)
    {

    }

    size_type
    get_size(
      void
      ) const
    {
      return buffer_ref<char>::is_empty()
        ? 0
        : buffer_ref<char>::get_size() - 1;
    }

    operator string(
      void
      ) const
    {
      return string(begin(), get_size());
    }

    friend bool
    operator==(
      const string_ref& lhs,
      const string& rhs
      )
    {
      return lhs.equals(rhs);
    }

    friend bool
    operator!=(
      const string_ref& lhs,
      const string& rhs
      )
    {
      return !(lhs == rhs);
    }

    friend bool
    operator==(
      const string_ref& lhs,
      const string_ref& rhs
      )
    {
      return lhs.equals(rhs);
    }

    friend bool
    operator!=(
      const string_ref& lhs,
      const string_ref& rhs
      )
    {
      return !(lhs == rhs);
    }

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
