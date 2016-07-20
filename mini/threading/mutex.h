#pragma once
#include <mini/common.h>

#include <windows.h>

#define mini_lock(mutex_instance)                           \
  if (::mini::threading::mutex_holder __h = mutex_instance) \
  {                                                         \
                                                            \
  }                                                         \
  else

namespace mini::threading {

class mutex
{
  public:
    MINI_MAKE_NONCOPYABLE(mutex);

    mutex(
      void
      );

    ~mutex(
      void
      );

    void
    acquire(
      void
      );

    void
    release(
      void
      );

  private:
    CRITICAL_SECTION _critical_section;
};

class mutex_holder
{
  public:
    MINI_MAKE_NONCOPYABLE(mutex_holder);

    mutex_holder(
      mutex& mutex
      );

    ~mutex_holder(
      void
      );

    operator bool(
      void
      );

  private:
    mutex& _mutex;
};

}
