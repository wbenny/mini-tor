#pragma once
#include <mini/common.h>
#include <mini/time.h>

#include <windows.h>

namespace mini::threading {

enum class reset_type : uint8_t
{
  manual_reset,
  auto_reset,
};

enum class wait_result
{
  success,
  timeout,
  error,
};

class event
{
  public:
    MINI_MAKE_NONCOPYABLE(event);

    event(
      reset_type type = reset_type::manual_reset,
      bool initial_state = false
      );

    ~event(
      void
      );

    void
    set(
      void
      );

    void
    reset(
      void
      );

    wait_result
    wait(
      timeout_type timeout = wait_infinite
      );

    bool
    is_signaled(
      void
      );

  private:
    HANDLE _event;
};

}
