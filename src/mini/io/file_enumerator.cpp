#include "file_enumerator.h"

namespace mini::io {

//
// file_iterator.
//

const file_iterator file_iterator::invalid;

file_iterator::file_iterator(
  void
  )
  : _is_invalid(true)
{

}

file_iterator::file_iterator(
  const string_ref path
  )
{
  _find_handle = FindFirstFileA(path.get_buffer(), &_find_data);
  _is_invalid = _find_handle == INVALID_HANDLE_VALUE;
}

file_iterator&
file_iterator::operator++(
  void
  )
{
  if (!_is_invalid)
  {
    _is_invalid = !FindNextFileA(_find_handle, &_find_data);
  }

  return *this;
}

file_iterator
file_iterator::operator++(
  int
  )
{
  file_iterator result = *this;
  operator++();
  return result;
}

bool
file_iterator::operator==(
  const file_iterator& other
  )
{
  return _is_invalid && other._is_invalid;
}

bool
file_iterator::operator!=(
  const file_iterator& other
  )
{
  return !operator==(other);
}

file_info&
file_iterator::operator*(
  void
  )
{
  _cached_file_info = file_info {
    _find_data.cFileName,
      static_cast<file_size_type>(_find_data.nFileSizeHigh) << 32 |
      static_cast<file_size_type>(_find_data.nFileSizeLow),
    _find_data.dwFileAttributes
  };

  return _cached_file_info;
}

file_info*
file_iterator::operator->(
  void
  )
{
  return &_cached_file_info;
}

//
// file_enumerator.
//

file_enumerator::file_enumerator(
  const string_ref path
  )
  : _path(path)
{

}

file_iterator
file_enumerator::begin(
  void
  ) const
{
  return file_iterator(_path);
}

file_iterator
file_enumerator::end(
  void
  ) const
{
  return file_iterator::invalid;
}

}
