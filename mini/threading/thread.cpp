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
  : _thread_handle(INVALID_HANDLE_VALUE)
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
  if (_thread_handle != INVALID_HANDLE_VALUE)
  {
    TerminateThread(_thread_handle, 0);
    CloseHandle(_thread_handle);

    _thread_handle = INVALID_HANDLE_VALUE;
  }
}

void
thread::join(
  timeout_type timeout
  )
{
  WaitForSingleObject(
    _thread_handle,
    (DWORD)timeout
    );
}

//
// observers.
//

thread::id
thread::get_id(
  void
  )
{
  return (id)GetCurrentThreadId();
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
