#include "file_attributes.h"

namespace mini::io {

const file_attributes file_attributes::invalid;

file_attributes::file_attributes(
  void
  )
  : _attributes(INVALID_FILE_ATTRIBUTES)
{

}

file_attributes::file_attributes(
  uint32_t attributes
  )
  : _attributes(attributes)
{

}

bool
file_attributes::is_hidden(
  void
  ) const
{
  return !!(_attributes & FILE_ATTRIBUTE_HIDDEN);
}

bool
file_attributes::is_system(
  void
  ) const
{
  return !!(_attributes & FILE_ATTRIBUTE_SYSTEM);
}

bool
file_attributes::is_file(
  void
  ) const
{
  return !!(_attributes & FILE_ATTRIBUTE_NORMAL) &&
    !(_attributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool
file_attributes::is_directory(
  void
  ) const
{
  return !!(_attributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool
file_attributes::operator!=(
  const file_attributes& other
  ) const
{
  return !operator==(other);
}

bool
file_attributes::operator==(
  const file_attributes& other
  ) const
{
  return _attributes == other._attributes;
}

}
