#include "path.h"

namespace mini::io {

string
path::combine(
  const string_ref p1,
  const string_ref p2
  )
{
  string result = p1;

  if (!p1.ends_with(directory_separator) && !p1.ends_with(alternative_directory_separator))
  {
    result += directory_separator;
  }

  result += p2;

  return result;
}

string_collection
path::split(
  const string_ref p
  )
{
  //
  // TODO:
  // take alternative_directory_separator into account.
  //
  auto result = p.split(directory_separator);

  //
  // always end drive letter with '\'.
  //
  if (result.get_size() > 0 && result[0].get_size() > 1)
  {
    string& drive_letter = result[0];

    if (drive_letter[1] == ':')
    {
      drive_letter += directory_separator;
    }
  }

  return result;
}

string_ref
path::get_file_name(
  const string_ref p
  )
{
  size_type name_offset = p.last_index_of(directory_separator);

  if (name_offset == string_ref::not_found)
  {
    name_offset = p.last_index_of(alternative_directory_separator);
  }

  if (name_offset == string_ref::not_found || (name_offset + 1) >= p.get_size())
  {
    return string_ref();
  }

  return p.substring(name_offset + 1);
}

string_ref
path::get_directory_name(
  const string_ref p
  )
{
  size_type name_offset = p.last_index_of(directory_separator);

  if (name_offset == string_ref::not_found)
  {
    name_offset = p.last_index_of(alternative_directory_separator);
  }

  return p.substring(0, name_offset);
}

string_ref
path::get_filename_without_extension(
  const string_ref p
  )
{
  size_type name_offset = p.last_index_of(directory_separator);

  if (name_offset == string_ref::not_found)
  {
    name_offset = p.last_index_of(alternative_directory_separator);
  }

  if (name_offset == string_ref::not_found || (name_offset + 1) >= p.get_size())
  {
    return string_ref();
  }

  string_ref result = p.substring(name_offset + 1);
  size_type extension_offset = result.last_index_of(extension_separator);

  return result.substring(0, extension_offset);
}

string_ref
path::get_extension(
  const string_ref p
  )
{
  string_ref file_name = get_file_name(p);
  size_type extension_offset = file_name.last_index_of(extension_separator);

  if (extension_offset != string_ref::not_found)
  {
    return file_name.substring(extension_offset);
  }

  return string_ref();
}

}
