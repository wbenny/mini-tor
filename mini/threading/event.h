#pragma once
#include <mini/common.h>
#include <mini/time.h>
#include <mini/buffer_ref.h>

#include <windows.h>

namespace mini::threading {

enum class reset_type : uint8_t
{
  auto_reset,
  manual_reset,
};

enum class wait_result : DWORD
{
  success   = WAIT_OBJECT_0,
  abandoned = WAIT_ABANDONED_0,
  timeout   = WAIT_TIMEOUT,
  failed    = WAIT_FAILED,
};

#define mini_wait_success(result) ((result) == ::mini::threading::wait_result::success)

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

    static wait_result
    wait_for_all(
      buffer_ref<const event*> events,
      timeout_type timeout = wait_infinite
      );

    static wait_result
    wait_for_any(
      buffer_ref<const event*> events,
      timeout_type timeout = wait_infinite
      );

    static int
    index_from_wait_result(
      wait_result result
      );

    bool
    is_signaled(
      void
      );

  private:
    HANDLE _event;
};

}
