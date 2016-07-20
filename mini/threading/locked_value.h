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
      );

    void
    set_value(
      const T& value
      );

    wait_result
    wait_for_value(
      const T& value,
      timeout_type timeout = wait_infinite
      );

  private:
    mutex _mutex;
    event _event;
    T _value;
};

}

#include "locked_value.inl"
