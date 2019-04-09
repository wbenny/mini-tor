#include "console.h"

#include <mini/threading/mutex.h>

//
// functions
//

#ifndef MINI_MSVCRT_LIB
# include <cstdio>
# include <cstdlib>
#else

extern "C" {

int sscanf(
  const char* buffer,
  const char* format,
  ...
  );

int printf(
  const char* format,
  ...
  );

}

#endif

namespace mini::console {

threading::mutex g_console_output_mutex;

void
write(
  const char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  write_args(format, args);
  va_end(args);
}

void
write_with_color(
  WORD color,
  const char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  write_with_color_args(color, format, args);
  va_end(args);
}

void
write_args(
  const char* format,
  va_list args
  )
{
  mini_lock(g_console_output_mutex)
  {
    vprintf(format, args);
  }
}

void
write_with_color_args(
  WORD color,
  const char* format,
  va_list args
  )
{
  mini_lock(g_console_output_mutex)
  {
    WORD previous_color = get_color();
    set_color(color);
    vprintf(format, args);
    set_color(previous_color);
  }
}


WORD
get_color(
  void
  )
{
  CONSOLE_SCREEN_BUFFER_INFO console_info;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console_info);

  return console_info.wAttributes;
}

void
set_color(
  WORD color
  )
{
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

}
