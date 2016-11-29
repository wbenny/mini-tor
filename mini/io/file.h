#pragma once
#include <mini/string.h>
#include <mini/collections/list.h>

namespace mini::io::file {

bool
exists(
  const string_ref path
  );

string
read_to_string(
  const string_ref path
  );

collections::list<string>
read_all_lines(
  const string_ref path
  );

void
write_from_string(
  const string_ref path,
  const byte_buffer_ref content
  );

}
