#pragma once
#include <mini/common.h>

namespace mini::io {

class stream
{
  public:
    //
    // this value is returned when a stream has been closed during read.
    //
    static constexpr size_t closed = (size_t)0;

    //
    // this value is returned when a stream has encountered an error.
    //
    static constexpr size_t error  = (size_t)-1;

    static bool
    success(
      size_t return_value
      )
    {
      return
        return_value != closed &&
        return_value != error;
    }

    enum seek_origin
    {
      begin,
      current,
      end,
    };

    virtual ~stream(
      void
      )
    {

    }

    virtual bool
    can_read(
      void
      ) const = 0;

    virtual bool
    can_write(
      void
      ) const = 0;

    virtual bool
    can_seek(
      void
      ) const = 0;

    virtual size_t
    read(
      void* buffer,
      size_t size
      ) = 0;

    virtual size_t
    write(
      const void* buffer,
      size_t size
      ) = 0;

    virtual size_t
    seek(
      intptr_t offset,
      seek_origin origin = seek_origin::current
      ) = 0;

    virtual void
    flush(
      void
      ) = 0;

    virtual size_t
    get_size(
      void
      ) const = 0;

    virtual size_t
    get_position(
      void
      ) const = 0;
};

}
