#include "string.h"
#include "memory.h"
#include "common.h"

#include <memory.h>
#include <mini/memory.h>
#include <mini/string_hash.h>

namespace mini {

//
// constructors.
//

string::string(
  void
  )
{

}

string::string(
  const string& other
  )
{
  assign(other);
}

string::string(
  string&& other
  )
{
  swap(other);
}

string::string(
  size_type initial_size
  )
{
  resize(initial_size);
}

string::string(
  const value_type* other,
  size_type size
  )
{
  assign(other, size);
}

//
// destructor.
//

string::~string(
  void
  )
{

}

//
// assign operators.
//

string&
string::operator=(
  const string& other
  )
{
  return assign(other);
}

string&
string::operator=(
  string&& other
  )
{
  swap(other);
  return *this;
}

string&
string::operator+=(
  const string& other
  )
{
  return append(other);
}

//
// swap.
//

void
string::swap(
  string& other
  )
{
  _buffer.swap(other._buffer);
}

//
// element access.
//

typename string::reference
string::operator[](
  size_type index
  )
{
  return at(index);
}

typename string::const_reference
string::operator[](
  size_type index
  ) const
{
  return at(index);
}

typename string::reference
string::at(
  size_type index
  )
{
  return _buffer.get_buffer()[index];
}

typename string::const_reference
string::at(
  size_type index
  ) const
{
  return _buffer.get_buffer()[index];
}

typename string::value_type*
string::get_buffer(
  void
  )
{
  return _buffer.get_buffer();
}

const typename string::value_type*
string::get_buffer(
  void
  ) const
{
  return _buffer.get_buffer();
}

//
// iterators.
//

typename string::iterator
string::begin()
{
  return _buffer.begin();
}

typename string::const_iterator
string::begin(
  void
  ) const
{
  return _buffer.begin();
}

typename string::iterator
string::end(
  void
  )
{
  return _buffer.end();
}

typename string::const_iterator
string::end(
  void
  ) const
{
  return _buffer.end();
}

//
// capacity
//

bool
string::is_empty(
  void
  ) const
{
  return _buffer.is_empty();
}

typename string::size_type
string::get_size(
  void
  ) const
{
  return _buffer.get_size() ? _buffer.get_size() - 1 : 0;
}

void
string::resize(
  size_type new_size,
  value_type item
  )
{
  _buffer.resize(new_size + 1, item);
  _buffer[get_size()] = 0;
}


typename string::size_type
string::get_capacity(
  void
  ) const
{
  return _buffer.get_capacity();
}

void
string::reserve(
  size_type new_capacity
  )
{
  _buffer.reserve(new_capacity);
}

//
// lookup.
//

typename string::size_type
string::index_of(
  const string_ref item,
  size_type from_offset
  ) const
{
  const value_type* ptr = reinterpret_cast<const value_type*>(memory::find(
    _buffer.get_buffer() + from_offset,
    get_size() - from_offset,
    item.get_buffer(),
    item.get_size()));

  if (ptr)
  {
    return ptr - _buffer.get_buffer();
  }

  return not_found;
}

bool
string::contains(
  const string_ref item
  ) const
{
  return index_of(item) != not_found;
}

bool
string::starts_with(
  const string_ref item
  ) const
{
  return index_of(item) == 0;
}

bool
string::ends_with(
  const string_ref item
  ) const
{
  return index_of(item) == (get_size() - item.get_size());
}

//
// operations.
//

bool
string::equals(
  const string& other
  ) const
{
  return
    get_size() == other.get_size() &&
    compare(other) == 0;
}

int
string::compare(
  const string& other
  ) const
{
  return memcmp(_buffer.get_buffer(), other.get_buffer(), get_size());
}

string
string::substring(
  size_type offset
  ) const
{
  return substring(offset, size_type(-1));
}

string
string::substring(
  size_type offset,
  size_type length
  ) const
{
  string result;
  length = min(get_size() - offset, length);
  result.resize(length);
  memcpy(result.get_buffer(), get_buffer() + offset, length);

  return result;
}

string_collection
string::split(
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

void
string::from_int(
  int value
  )
{
  value_type tmp[40];
  sprintf(tmp, "%d", value);

  *this = tmp;
}

int
string::to_int(
  void
  ) const
{
  int result = 0;
  sscanf(get_buffer(), "%d", &result);

  return result;
}

//
// modifiers.
//

string&
string::assign(
  const value_type* other,
  size_type size
  )
{
  size = size == zero_terminated
    ? strlen(other)
    : size;

  resize(size);
  memcpy(_buffer.get_buffer(), other, size);

  return *this;
}

string&
string::assign(
  const string& other
  )
{
  return assign(other.get_buffer(), other.get_size());
}

string&
string::assign(
  string&& other
  )
{
  swap(other);

  return *this;
}

string&
string::append(
  value_type other
  )
{
  value_type tmp[2] = { other, '\0' };

  return append(tmp);
}

string&
string::append(
  const value_type* other,
  size_type size
  )
{
  size = size == zero_terminated
    ? strlen(other)
    : size;

  size_type old_size = get_size();
  _buffer.resize(old_size + size + 1);

  memcpy(get_buffer() + old_size, other, size);
  _buffer[old_size + size] = '\0';

  return *this;
}

string&
string::append(
  const string& other
  )
{
  return append(other.get_buffer(), other.get_size());
}

void
string::clear(
  void
  )
{
  _buffer.clear();
  _buffer[0] = '\0';
}

//
// conversion operator.
//
string::operator byte_buffer_ref(
  void
  ) const
{
  return byte_buffer_ref(_buffer.begin(), _buffer.end());
}

string::operator mutable_byte_buffer_ref(
  void
  )
{
  return mutable_byte_buffer_ref(_buffer.begin(), _buffer.end());
}

string::operator string_ref(
  void
  ) const
{
  return string_ref(_buffer.begin(), _buffer.begin() + _buffer.get_size());
}

string::operator mutable_string_ref(
  void
  )
{
  return mutable_string_ref(_buffer.begin(), _buffer.begin() + _buffer.get_size());
}

string::operator string_hash(
  void
  ) const
{
  return string_hash(_buffer.begin());
}

//
// non-member operations.
//

bool
operator==(
  const string& lhs,
  const string& rhs
  )
{
  return lhs.equals(rhs);
}

bool
operator!=(
  const string& lhs,
  const string& rhs
  )
{
  return !lhs.equals(rhs);
}

string
operator+(
  const string& lhs,
  const string& rhs
  )
{
  string result(lhs);
  result += rhs;
  return result;
}

//
// non-class functions.
//

void
swap(
  string& lhs,
  string& rhs
)
{
  lhs.swap(rhs);
}

}
