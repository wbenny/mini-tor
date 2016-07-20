#pragma once
#include "mutex.h"

namespace mini::threading {

//
// mutex implementation.
//

mutex::mutex(
  void
  )
{
  InitializeCriticalSection(&_critical_section);
}

mutex::~mutex(
  void
  )
{
  DeleteCriticalSection(&_critical_section);
}

void
mutex::acquire(
  void
  )
{
  EnterCriticalSection(&_critical_section);
}

void
mutex::release(
  void
  )
{
  LeaveCriticalSection(&_critical_section);
}

//
// mutex holder implementation.
//

mutex_holder::mutex_holder(
  mutex& mutex
  )
  : _mutex(mutex)
{
  _mutex.acquire();
}

mutex_holder::~mutex_holder(
  void
  )
{
  _mutex.release();
}

mutex_holder::operator bool(
  void
  )
{
  return false;
}

}
