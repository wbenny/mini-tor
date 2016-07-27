#pragma once
#include "string_ref.h"

namespace mini {

string_ref::string_ref(
  char value
  )
  : buffer_ref<char>(_internal_char_buffer, _internal_char_buffer + 2)
{
  _internal_char_buffer[0] = value;
  _internal_char_buffer[1] = '\0';
}

template <
  size_t N
>
constexpr string_ref::string_ref(
  const char (&value)[N]
  )
  : buffer_ref<char>(value, value + N)
{

}

string_ref::string_ref(
  const char* value
  )
  : buffer_ref<char>(value, value + strlen(value) + 1)
{

}

size_type
string_ref::get_size(
  void
  ) const
{
  return buffer_ref<char>::is_empty()
    ? 0
    : buffer_ref<char>::get_size() - 1;
}

string_ref::operator string(
  void
  ) const
{
  return string(begin(), get_size());
}

bool
operator==(
  const string_ref& lhs,
  const string& rhs
  )
{
  return lhs.equals(rhs);
}

bool
operator!=(
  const string_ref& lhs,
  const string& rhs
  )
{
  return !(lhs == rhs);
}

bool
operator==(
  const string_ref& lhs,
  const string_ref& rhs
  )
{
  return lhs.equals(rhs);
}

bool
operator!=(
  const string_ref& lhs,
  const string_ref& rhs
  )
{
  return !(lhs == rhs);
}

}
