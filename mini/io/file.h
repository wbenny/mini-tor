#pragma once
#include <mini/string.h>
#include <mini/collections/list.h>
#include <mini/io/file_stream.h>

#include <shlwapi.h>

namespace mini::io::file {

static bool
exists(
  const string_ref path
  )
{
  return PathFileExists(path.get_buffer()) == TRUE;
}

static string
read_to_string(
  const string_ref path
  )
{
  io::file_stream f(path, io::file_access::read);

  string result;
  result.resize(f.get_size());
  f.read(&result[0], f.get_size());

  return result;
}

static collections::list<string>
read_all_lines(
  const string_ref path
  )
{
  return read_to_string(path).split("\n");
}

static void
write_from_string(
  const string_ref path,
  const byte_buffer_ref content
  )
{
  io::file_stream f(path, io::file_access::write);
  f.write(content.get_buffer(), content.get_size());
}


}
