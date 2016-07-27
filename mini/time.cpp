#include "time.h"

#include <windows.h>
#include <winternl.h>

namespace mini {

//
// constructors.
//

time::time(
  void
  )
  : time(0)
{

}

time::time(
  uint32_t value
  )
  : _timestamp(value)
{

}

//
// conversion.
//

void
time::parse(
  const string_ref value
  )
{
  //
  // must be in format "2016-06-14 01:00:00"
  //

  SYSTEMTIME system_time = { 0 };
  sscanf(
    value.get_buffer(),
    "%04hd-%02hd-%02hd %02hd:%02hd:%02hd",
    &system_time.wYear,
    &system_time.wMonth,
    &system_time.wDay,
    &system_time.wHour,
    &system_time.wMinute,
    &system_time.wSecond);

  LARGE_INTEGER file_time;
  SystemTimeToFileTime(
    &system_time,
    (FILETIME*)&file_time
    );

  time result;
  RtlTimeToSecondsSince1970(
    &file_time,
    (ULONG*)&_timestamp
    );
}

uint32_t
time::to_timestamp(
  void
  ) const
{
  return _timestamp;
}

//
// static methods.
//

time
time::now(
  void
  )
{
  LARGE_INTEGER file_time;
  GetSystemTimeAsFileTime((FILETIME*)&file_time);

  time result;
  RtlTimeToSecondsSince1970(&file_time, (ULONG*)&result._timestamp);

  return result;
}

timestamp_type
time::timestamp(
  void
  )
{
  return GetTickCount();
}

//
// operators.
//

bool
operator<(
  const time& lhs,
  const time& rhs
  )
{
  return lhs._timestamp < rhs._timestamp;
}

bool
operator>(
  const time& lhs,
  const time& rhs
  )
{
  return lhs._timestamp > rhs._timestamp;
}

};

