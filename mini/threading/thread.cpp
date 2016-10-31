#pragma once
#include "thread.h"

namespace mini::threading {

namespace detail {

void
thread_dispatcher(
  thread* thread_instance
  )
{
  thread_instance->thread_procedure();
}

DWORD WINAPI
native_thread_dispatcher(
  LPVOID lpParam
  )
{
  thread_dispatcher(reinterpret_cast<thread*>(lpParam));
  return 0;
}

}

//
// constructors.
//

thread::thread(
  void
  )
  //
  // it's quite important to not use the INVALID_HANDLE_VALUE constant,
  // because the value is same as "current thread" pseudo-handle.
  //
  : _thread_handle(0)
  , _thread_id(0)
{

}

thread::thread(
  thread&& other
  )
  : thread()
{
  swap(other);
}

thread::thread(
  current_thread_tag&
  )
  : _thread_handle(GetCurrentThread())
  , _thread_id(GetCurrentThreadId())
{

}

//
// destructor
//

thread::~thread(
  void
  )
{
  stop();
}

//
// swap.
//

void
thread::swap(
  thread& other
  )
{
  mini::swap(_thread_handle, other._thread_handle);
  mini::swap(_thread_id, other._thread_id);
}

//
// operations.
//

void
thread::start(
  void
  )
{
  if (_thread_id != 0)
  {
    //
    // thread is already running.
    //
    return;
  }

  _thread_handle = CreateThread(
    NULL,                               // default security attributes
    0,                                  // use default stack size
    &detail::native_thread_dispatcher,  // thread function name
    (LPVOID)this,                       // argument to thread function
    0,                                  // use default creation flags
    &_thread_id);                       // returns the thread identifier
}

void
thread::stop(
  void
  )
{
  if (_thread_handle == 0)
  {
    //
    // thread is not running or is already terminated.
    //
    return;
  }

  // bool is_terminated = WaitForSingleObject(_thread_handle, 0) == WAIT_OBJECT_0;

  TerminateThread(_thread_handle, 0);
  CloseHandle(_thread_handle);

  _thread_handle = 0;
  _thread_id = 0;
}

wait_result
thread::join(
  timeout_type timeout
  )
{
  return static_cast<wait_result>(WaitForSingleObject(
    _thread_handle,
    (DWORD)timeout
    ));
}

//
// observers.
//

thread::id
thread::get_id(
  void
  ) const
{
  return _thread_id;
}

bool
thread::is_running(
  void
  ) const
{
  return
    _thread_id != 0 &&
    WaitForSingleObject(_thread_handle, 0) == WAIT_TIMEOUT;
}

//
// static methods.
//

thread
thread::get_current_thread(
  void
  )
{
  return thread(current_thread_tag());
}

void
thread::sleep(
  timeout_type milliseconds
  )
{
  Sleep(milliseconds);
}

//
// virtual methods.
//

void
thread::thread_procedure(
  void
  )
{

}

void
swap(
  thread& lhs,
  thread& rhs
  )
{
  lhs.swap(rhs);
}

}
