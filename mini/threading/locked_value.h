#pragma once
#include "mutex.h"
#include "event.h"

namespace mini::threading {

template <
  typename T
>
class locked_value
{
  public:
    MINI_MAKE_NONCOPYABLE(locked_value);

    locked_value(
      const T& initial_value = T()
      );

    ~locked_value(
      void
      ) = default;

    const T&
    get_value(
      void
      ) const;

    void
    set_value(
      const T& value
      );

    wait_result
    wait_for_value(
      const T& value,
      timeout_type timeout = wait_infinite
      );

    wait_result
    wait_for_change(
      timeout_type timeout = wait_infinite
      );

    void
    cancel_all_waits(
      void
      );

  private:
    mutable mutex _mutex;
    event _event;
    event _cancel_event;
    T _value;
};

}

#include "locked_value.inl"
