#pragma once
#include <mini/common.h>
#include <mini/time.h>

#include <windows.h>
#include <cstdint>

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
      );

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

    void
    join(
      timeout_type timeout = wait_infinite
      );

    //
    // observers.
    //

    id
    get_id(
      void
      );

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

    HANDLE _thread_handle;
    DWORD _thread_id;
};

void
swap(
  thread& lhs,
  thread& rhs
  );

}
