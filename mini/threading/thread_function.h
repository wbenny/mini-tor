#pragma once
#include "thread.h"
#include <mini/function.h>

namespace mini::threading {

class thread_function
  : public thread
{
  public:
    MINI_MAKE_NONCOPYABLE(thread_function);

    thread_function(
      const function<void()>& functor
      )
      : _functor(functor)
    {

    }

    thread_function(
      function<void()>&& functor
      )
      : _functor(std::move(functor))
    {

    }

  private:
    void
    thread_procedure() override
    {
      _functor();
    }

    function<void()> _functor;
};

}
