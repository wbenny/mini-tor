#pragma once
#include <mini/common.h>

#include <windows.h>

#define MINI_LOG_ENABLED 1

#if defined(_DEBUG) || defined(MINI_LOG_ENABLED)

//
// variadic macro rape
//

#define MINI_MACRO_EVAL(...)                        MINI_MACRO_EVAL_IMPL(MINI_MACRO_EVAL_IMPL(MINI_MACRO_EVAL_IMPL(__VA_ARGS__)))
#define MINI_MACRO_EVAL_IMPL(...)                   __VA_ARGS__

#define MINI_MACRO_EXPAND(expr)                     expr
#define MINI_MACRO_DISPATCHER(func, ...)            MINI_MACRO_EXPAND(MINI_MACRO_DISPATCHER2(func, MINI_VA_NUM_ARGS(func ## _NUM_ARGS_TABLE, __VA_ARGS__)))
#define MINI_MACRO_DISPATCHER2(func, nargs)         MINI_MACRO_DISPATCHER3(func, nargs)
#define MINI_MACRO_DISPATCHER3(func, nargs)         func ## nargs

#define MINI_VA_NUM_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...)   N
#define MINI_VA_NUM_ARGS(table, ...)                MINI_MACRO_EXPAND(MINI_VA_NUM_ARGS_IMPL(__VA_ARGS__, table))

#define MINI_VA_FUNCTION(function, ...)             MINI_MACRO_EXPAND(MINI_MACRO_DISPATCHER(function, __VA_ARGS__)(__VA_ARGS__))

//
// MINI_LOG
//

#define MINI_LOG_NUM_ARGS_TABLE                     1, 1, 1, 1, 1, 1, 1, 0, 0, 0

#define MINI_LOG0(level, format)                    ::mini::log.log(level, format "\n")
#define MINI_LOG1(level, format, ...)               ::mini::log.log(level, format "\n", __VA_ARGS__)

//
// mini_log
//

# define mini_log(...)                              MINI_VA_FUNCTION(MINI_LOG, ::mini::logger::level::info,    __VA_ARGS__)
# define mini_debug(...)                            MINI_VA_FUNCTION(MINI_LOG, ::mini::logger::level::debug,   __VA_ARGS__)
# define mini_info(...)                             MINI_VA_FUNCTION(MINI_LOG, ::mini::logger::level::info,    __VA_ARGS__)
# define mini_warning(...)                          MINI_VA_FUNCTION(MINI_LOG, ::mini::logger::level::warning, __VA_ARGS__)
# define mini_error(...)                            MINI_VA_FUNCTION(MINI_LOG, ::mini::logger::level::error,   __VA_ARGS__)
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

extern logger log;

}
