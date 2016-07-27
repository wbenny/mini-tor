#include "logger.h"
#include "console.h"

namespace mini {

static constexpr WORD level_colors[] = {
  FOREGROUND_INTENSITY | FOREGROUND_GREEN /* | FOREGROUND_BLUE */,  // debug
  FOREGROUND_INTENSITY | FOREGROUND_GREEN,                    // info
  FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED,   // warning
  FOREGROUND_INTENSITY | FOREGROUND_RED,                      // error
};

void
logger::log(
  level l,
  const char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  log_args(l, format, args);
  va_end(args);
}

void
logger::log_args(
  level l,
  const char* format,
  va_list args
  )
{
  if (l >= _level)
  {
    console::write_with_color_args(level_colors[(int)l], format, args);
  }
}

void
logger::debug(
  const char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  log(level::debug, format, args);
  va_end(args);
}

void
logger::info(
  const char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  log(level::info, format, args);
  va_end(args);
}

void
logger::warning(
  const char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  log(level::warning, format, args);
  va_end(args);
}

void
logger::error(
  const char* format,
  ...
  )
{
  va_list args;
  va_start(args, format);
  log(level::error, format, args);
  va_end(args);
}

logger::level
logger::get_level(
  void
  ) const
{
  return _level;
}

void
logger::set_level(
  level new_level
  )
{
  _level = new_level;
}

}
