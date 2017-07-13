#pragma once
#include "file_attributes.h"

#include <mini/string.h>
#include <mini/collections/list.h>

namespace mini::io {

class file
{
  public:
    static file_attributes
    get_attributes(
        const string_ref path
      );

    static bool
    exists(
      const string_ref path
      );

    static string
    read_to_string(
      const string_ref path
      );

    static collections::list<string>
    read_all_lines(
      const string_ref path
      );

    static void
    write_from_string(
      const string_ref path,
      const byte_buffer_ref content
      );
};

}
