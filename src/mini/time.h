#pragma once
#include <mini/string.h>

namespace mini {

//
// number of seconds since midnight 1. 1. 1970.
//
using time_type = uint32_t;

//
// timeout type - number of milliseconds.
//
using timeout_type = int32_t;
static const timeout_type no_wait = 0;
static const timeout_type wait_infinite = (timeout_type)-1;

using timestamp_type = uint32_t;

class time
{
  public:
    //
    // constructors.
    //

    time(
      void
      );

    time(
      uint32_t value
      );

    time(
      const time& other
      ) = default;

    //
    // destructor.
    //

    ~time(
      void
      ) = default;

    //
    // conversion.
    //

    void
    parse(
      const string_ref value
      );

    time_type
    to_timestamp(
      void
      ) const;

    //
    // static methods.
    //

    static time
    now(
      void
      );

    static timestamp_type
    timestamp(
      void
      );

    //
    // operators.
    //

    friend bool
    operator==(
      const time& lhs,
      const time& rhs
      );

    friend bool
    operator!=(
      const time& lhs,
      const time& rhs
      );

    friend bool
    operator<(
      const time& lhs,
      const time& rhs
      );

    friend bool
    operator>(
      const time& lhs,
      const time& rhs
      );

    friend bool
    operator<=(
      const time& lhs,
      const time& rhs
      );

    friend bool
    operator>=(
      const time& lhs,
      const time& rhs
      );

  private:
    time_type _timestamp;
};

}
