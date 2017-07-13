#include "common.h"
#include <mini/logger.h>

namespace mini {

void
assert(
  int expression,
  const char* expression_str,
  const char* filename,
  int line
  )
{
#ifndef MINI_CONFIG_DEBUG
  MINI_UNREFERENCED(expression_str);
  MINI_UNREFERENCED(filename);
  MINI_UNREFERENCED(line);
#endif

  if (!expression)
  {
    mini_error("!! assertion failed !! %s at %s:%i)", expression_str, filename, line);
    __debugbreak();
  }
}

}
