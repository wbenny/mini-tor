#pragma once
#include <mini/common.h>

#include <windows.h>

#define mini_lock(mutex_instance)                             \
  if (::mini::threading::mutex_holder __h = mutex_instance)   \
  {                                                           \
    MINI_UNREACHABLE;                                         \
  }                                                           \
  else

#define mini_unlock(mutex_instance)                           \
  if (::mini::threading::mutex_unholder __h = mutex_instance) \
  {                                                           \
    MINI_UNREACHABLE;                                         \
  }                                                           \
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
      ) const;

  private:
    mutex& _mutex;
};

class mutex_unholder
{
  public:
    MINI_MAKE_NONCOPYABLE(mutex_unholder);

    mutex_unholder(
      mutex& mutex
      );

    ~mutex_unholder(
      void
      );

    operator bool(
      void
      ) const;

  private:
    mutex& _mutex;
};

}
