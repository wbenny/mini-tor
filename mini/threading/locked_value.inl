#pragma once
#include "locked_value.h"

#include <mini/time.h>

namespace mini::threading {

template <
  typename T
>
locked_value<T>::locked_value(
  const T& initial_value
  )
  : _event(reset_type::auto_reset, false)
{
  _value = initial_value;
}

template <
  typename T
>
const T&
locked_value<T>::get_value(
  void
  )
{
  mini_lock(_mutex)
  {
    return _value;
  }

  MINI_UNREACHABLE;
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
    _value = value;
    _event.set();
  }
}

template <
  typename T
>
wait_result
locked_value<T>::wait_for_value(
  const T& value,
  timeout_type timeout = wait_infinite
  )
{
  timestamp_type start_timestamp = time::timestamp();
  timeout_type remaining_timeout = timeout;

  while (_value != value)
  {
    _mutex.release();
    _event.wait(remaining_timeout);
    _mutex.acquire();

    if (timeout != wait_infinite)
    {
      timestamp_type elapsed_milliseconds = time::timestamp() - start_timestamp;
      remaining_timeout = timeout - elapsed_milliseconds;
    }
  }

  return wait_result::success;
}


}
