#pragma once
#include <mini/common.h>
#include <mini/string.h>

#include <cstdarg>

namespace mini::console {

void
write(
  const char* format,
  ...
  );

void
write_with_color(
  WORD color,
  const char* format,
  ...
  );

void
write_args(
  const char* format,
  va_list args
  );

void
write_with_color_args(
  WORD color,
  const char* format,
  va_list args
  );

WORD
get_color(
  void
  );

void
set_color(
  WORD color
  );

}
