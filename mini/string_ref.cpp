#include "string_ref.h"
#include "algorithm.h"

namespace mini {

const string_ref string_ref::empty;

//
// constructors.
//

string_ref::string_ref(
  void
  )
  : buffer_ref<char>(_internal_char_buffer, _internal_char_buffer)
{
  _internal_char_buffer[0] = '\0';
}

string_ref::string_ref(
  char value
  )
  : buffer_ref<char>(_internal_char_buffer, _internal_char_buffer + 1)
{
  _internal_char_buffer[0] = value;
  _internal_char_buffer[1] = '\0';
}

template <
  size_type N
>
constexpr string_ref::string_ref(
  const char (&value)[N]
  )
  : buffer_ref<char>(value, value + N - 1)
{

}

string_ref::string_ref(
  const char* value
  )
  : buffer_ref<char>(value, value + strlen(value))
{

}

//
// capacity.
//

bool
string_ref::is_empty(
  void
  ) const
{
  return get_size() == 0;
}

//
// lookup.
//

size_type
string_ref::index_of(
  const string_ref item,
  size_type from_offset
  ) const
{
  const value_type* ptr = reinterpret_cast<const value_type*>(memory::find(
    buffer_ref<char>::get_buffer() + from_offset,
    get_size() - from_offset,
    item.get_buffer(),
    item.get_size()));

  if (ptr)
  {
    return ptr - buffer_ref<char>::get_buffer();
  }

  return not_found;
}

size_type
string_ref::last_index_of(
  const string_ref item,
  size_type from_offset
  ) const
{
  const value_type* ptr = reinterpret_cast<const value_type*>(memory::reverse_find(
    buffer_ref<char>::get_buffer() + from_offset,
    get_size() - from_offset,
    item.get_buffer(),
    item.get_size()));

  if (ptr)
  {
    return ptr - buffer_ref<char>::get_buffer();
  }

  return not_found;
}

bool
string_ref::contains(
  const string_ref item
  ) const
{
  return index_of(item) != not_found;
}

bool
string_ref::starts_with(
  const string_ref item
  ) const
{
  return index_of(item) == 0;
}

bool
string_ref::ends_with(
  const string_ref item
  ) const
{
  return index_of(item) == (get_size() - item.get_size());
}

//
// operations.
//

bool
string_ref::equals(
  const string_ref other
  ) const
{
  return
    get_size() == other.get_size() &&
    compare(other) == 0;
}

int
string_ref::compare(
  const string_ref other
  ) const
{
  return memory::compare(buffer_ref<char>::get_buffer(), other.get_buffer(), get_size());
}

string_ref
string_ref::substring(
  size_type offset
  ) const
{
  return substring(offset, size_type(-1));
}

string_ref
string_ref::substring(
  size_type offset,
  size_type length
  ) const
{
  length = algorithm::min(get_size() - offset, length);

  return string_ref(
    buffer_ref<char>::get_buffer() + offset,
    buffer_ref<char>::get_buffer() + offset + length);
}

string_collection
string_ref::split(
  const string_ref delimiter,
  size_type count
  ) const
{
  string_collection tokens;
  size_type previous = 0;
  size_type position = 0;

  do
  {
    position = index_of(delimiter, previous);

    if (position == not_found)
    {
      position = get_size();
    }

    string token = substring(previous, position - previous);

    if (!token.is_empty())
    {
      tokens.add(std::move(token));

      //
      // add the rest of the string.
      //
      if (tokens.get_size() == count)
      {
        token = substring(previous + position);
        tokens.add(std::move(token));
        break;
      }
    }

    previous = position + delimiter.get_size();
  } while (position < get_size() && previous < get_size());

  return tokens;
}

#if !defined(MINI_MODE_KERNEL)

int
string_ref::to_int(
  void
  ) const
{
  int result = 0;
  sscanf(get_buffer(), "%d", &result);

  return result;
}

#endif

bool
string_ref::is_zero_terminated(
  void
  ) const
{
  //
  // warning: this may actually trigger an memory access violation!
  //
  return get_buffer()[get_size() + 1] == '\0';
}

//
// conversion operators.
//

string_ref::operator string(
  void
  ) const
{
  return string(begin(), get_size());
}

string_ref::operator byte_buffer_ref(
  void
  ) const
{
  return byte_buffer_ref(begin(), end());
}

//
// non-member operations.
//

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

string
operator+(
  const string_ref& lhs,
  const string_ref& rhs
  )
{
  return string(lhs) + string(rhs);
}

}
