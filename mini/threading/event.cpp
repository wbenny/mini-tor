#pragma once
#include "event.h"
#include <mini/collections/list.h>

#include <type_traits>

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
  return static_cast<wait_result>(WaitForSingleObject(_event, timeout));
}

wait_result
event::wait_for_all(
  buffer_ref<const event*> events,
  timeout_type timeout
  )
{
  collections::list<HANDLE> handles;

  for (auto&& event : events)
  {
    handles.add(event->_event);
  }

  return static_cast<wait_result>(WaitForMultipleObjects(
    static_cast<DWORD>(handles.get_size()),
    //
    // dirty hack - as long as 'event' class
    // consists only from one HANDLE member,
    // this should work just fine.
    //
    &handles[0],
    TRUE,
    timeout));
}

wait_result
event::wait_for_any(
  buffer_ref<const event*> events,
  timeout_type timeout
  )
{
  collections::list<HANDLE> handles;

  for (auto&& event : events)
  {
    handles.add(event->_event);
  }

  return static_cast<wait_result>(WaitForMultipleObjects(
    static_cast<DWORD>(handles.get_size()),
    //
    // dirty hack - as long as 'event' class
    // consists only from one HANDLE member,
    // this should work just fine.
    //
    &handles[0],
    FALSE,
    timeout));
}

int
event::index_from_wait_result(
  wait_result result
  )
{
  return
    result == wait_result::timeout   ? -1 :
    result == wait_result::failed    ? -1 :
    result  > wait_result::abandoned ? (static_cast<int>(result) - static_cast<int>(wait_result::abandoned)) :
    static_cast<int>(result) - static_cast<int>(wait_result::success);
}

bool
event::is_signaled(
  void
  )
{
  return wait(0) == wait_result::success;
}

}
