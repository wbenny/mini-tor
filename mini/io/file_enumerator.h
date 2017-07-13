#pragma once
#include "file_attributes.h"

#include <mini/string.h>

#include <windows.h>

namespace mini::io {

//
// file_info.
//

struct file_info
{
  string full_name;
  file_size_type size;
  file_attributes attributes;
};

//
// file_iterator.
//

class file_iterator
{
  public:
    static const file_iterator invalid;

    file_iterator(
      void
      );

    file_iterator(
      const string_ref path
      );

    file_iterator&
    operator++(
      void
      );

    file_iterator
    operator++(
      int
      );

    bool
    operator==(
      const file_iterator& other
      );

    bool
    operator!=(
      const file_iterator& other
      );

    file_info&
    operator*(
      void
      );

    file_info*
    operator->(
      void
      );

  private:
    file_info _cached_file_info;
    WIN32_FIND_DATAA _find_data;
    HANDLE _find_handle;
    bool _is_invalid;
};

//
// file_enumerator.
//

class file_enumerator
{
  public:
    file_enumerator(
      const string_ref path
      );

    file_iterator
    begin(
      void
      ) const;

    file_iterator
    end(
      void
      ) const;

  private:
    string _path;
};

}
