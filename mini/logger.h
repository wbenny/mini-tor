#pragma once
#include <mini/common.h>

#include <windows.h>

#if 1 || _DEBUG
# define mini_log(format, ...)      ::mini::log.log(::mini::logger::level::info,    format "\n", __VA_ARGS__)
# define mini_debug(format, ...)    ::mini::log.log(::mini::logger::level::debug,   format "\n", __VA_ARGS__)
# define mini_info(format, ...)     ::mini::log.log(::mini::logger::level::info,    format "\n", __VA_ARGS__)
# define mini_warning(format, ...)  ::mini::log.log(::mini::logger::level::warning, format "\n", __VA_ARGS__)
# define mini_error(format, ...)    ::mini::log.log(::mini::logger::level::error,   format "\n", __VA_ARGS__)
#else
# define mini_log(format, ...)
# define mini_debug(format, ...)
# define mini_info(format, ...)
# define mini_warning(format, ...)
# define mini_error(format, ...)
#endif

namespace mini {

class logger
{
  public:
    MINI_MAKE_NONCOPYABLE(logger);

    logger(
      void
      ) = default;

    enum class level
    {
      debug,
      info,
      warning,
      error,
      off,
    };

    void
    log(
      level l,
      const char* format,
      ...
      );

    void
    log_args(
      level l,
      const char* format,
      va_list args
      );

    void
    debug(
      const char* format,
      ...
      );

    void
    info(
      const char* format,
      ...
      );

    void
    warning(
      const char* format,
      ...
      );

    void
    error(
      const char* format,
      ...
      );

    level
    get_level(
      void
      ) const;

    void
    set_level(
      level new_level
      );

  private:
    level _level = level::info;
};

static logger log;

}
