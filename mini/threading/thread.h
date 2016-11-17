#pragma once
#include "common.h"

#include <mini/common.h>
#include <mini/time.h>

#include <windows.h>

namespace mini::threading {

class thread;

namespace detail {
  void
  thread_dispatcher(
    thread* thread_instance
    );
}

class thread
{
  public:
    MINI_MAKE_NONCOPYABLE(thread);

    using id = uint32_t;
    struct current_thread_tag {};

    //
    // constructors.
    //

    thread(
      void
      ) = default;

    thread(
      thread&& other
      );

    thread(
      current_thread_tag&
      );

    //
    // destructor.
    //

    ~thread(
      void
      );

    //
    // swap.
    //

    void
    swap(
      thread& other
      );

    //
    // operations.
    //

    void
    start(
      void
      );

    void
    stop(
      void
      );

    wait_result
    join(
      timeout_type timeout = wait_infinite
      );

    //
    // observers.
    //

    id
    get_id(
      void
      ) const;

    bool
    is_alive(
      void
      ) const;

    //
    // static methods.
    //

    static thread
    get_current_thread(
      void
      );

    static void
    sleep(
      timeout_type milliseconds
      );

  protected:

    //
    // virtual methods.
    //

    virtual void
    thread_procedure(
      void
      );

  private:
    friend void detail::thread_dispatcher(
      thread* thread_instance
      );

    //
    // it's quite important to not use the INVALID_HANDLE_VALUE constant,
    // because the value is same as "current process" pseudo-handle.
    //
    HANDLE _thread_handle = 0;
    DWORD _thread_id = 0;
    bool _has_been_terminated = false;
};

void
swap(
  thread& lhs,
  thread& rhs
  );

}
