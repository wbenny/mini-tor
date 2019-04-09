#pragma once
#include "locked_value.h"

#include <mini/time.h>
#include <mini/logger.h>

namespace mini::threading {

template <
  typename T
>
locked_value<T>::locked_value(
  const T& initial_value
  )
  : _event(reset_type::auto_reset, false)
  , _cancel_event(reset_type::auto_reset, false)
{
  _value = initial_value;
}

template <
  typename T
>
const T&
locked_value<T>::get_value(
  void
  ) const
{
  mini_lock(_mutex)
  {
    return _value;
  }
}

template <
  typename T
>
void
locked_value<T>::set_value(
  const T& value
  )
{
  mini_lock(_mutex)
  {
    if (value != _value)
    {
      _value = value;
      _event.set();
    }
  }
}

template <
  typename T
>
wait_result
locked_value<T>::wait_for_value(
  const T& value,
  timeout_type timeout
  )
{
  timestamp_type start_timestamp = time::timestamp();
  timeout_type remaining_timeout = timeout;

  mini_lock(_mutex)
  {
    //
    // compare is performed under the lock.
    //
    while (_value != value)
    {
      wait_result result;

      //
      // unlock the value, so anyone can change it.
      //
      mini_unlock(_mutex)
      {
        result = wait_for_change(remaining_timeout);
      }

      //
      // check if we've been cancelled.
      // wait_for_change is waiting for _event & _cancel_event.
      // _cancel_event is at the index #1.
      //
      if (event::index_from_wait_result(result) == 1)
      {
        return wait_result::failed;
      }

      //
      // value has been changed, adjust the remaining timeout.
      //
      if (timeout != wait_infinite)
      {
        timestamp_type elapsed_milliseconds = time::timestamp() - start_timestamp;
        remaining_timeout = timeout - elapsed_milliseconds;

        if (remaining_timeout <= 0)
        {
          return wait_result::timeout;
        }
      }
    }
  }

  return wait_result::success;
}

template <
  typename T
>
wait_result
locked_value<T>::wait_for_change(
  timeout_type timeout
  )
{
  return event::wait_for_any({ &_event, &_cancel_event }, timeout);
}

template <
  typename T
>
void
locked_value<T>::cancel_all_waits(
  void
  )
{
  _cancel_event.set();
}

}
