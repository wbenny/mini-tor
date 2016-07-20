#pragma once
#include "event.h"

namespace mini::threading {

event::event(
  reset_type type,
  bool initial_state
  )
{
  _event = CreateEvent(NULL, (BOOL)type, initial_state, NULL);
}

event::~event(
  void
  )
{
  CloseHandle(_event);
}

void
event::set(
  void
  )
{
  SetEvent(_event);
}

void
event::reset(
  void
  )
{
  ResetEvent(_event);
}

wait_result
event::wait(
  timeout_type timeout
)
{
  const DWORD result = WaitForSingleObject(_event, timeout);

  return
    result == WAIT_OBJECT_0 ? wait_result::success :
    result == WAIT_TIMEOUT  ? wait_result::timeout :
                              wait_result::error   ;
}
bool
event::is_signaled(
  void
  )
{
  return wait(0) == wait_result::success;
}

}
