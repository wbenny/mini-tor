#pragma once
#include <mini/common.h>

namespace mini::io {

class stream
{
  public:
    //
    // this value is returned when a stream has been closed during read.
    //
    static constexpr size_type closed = (size_type)0;

    //
    // this value is returned when a stream has encountered an error.
    //
    static constexpr size_type error  = (size_type)-1;

    static bool
    success(
      size_type return_value
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

    virtual size_type
    read(
      void* buffer,
      size_type size
      ) = 0;

    virtual size_type
    write(
      const void* buffer,
      size_type size
      ) = 0;

    virtual size_type
    seek(
      intptr_t offset,
      seek_origin origin = seek_origin::current
      ) = 0;

    virtual void
    flush(
      void
      ) = 0;

    virtual size_type
    get_size(
      void
      ) const = 0;

    virtual size_type
    get_position(
      void
      ) const = 0;


    //
    // easier manipulation
    //

    virtual size_type
    read(
      mutable_byte_buffer_ref buffer
      )
    {
      return read(buffer.get_buffer(), buffer.get_size());
    }

    virtual size_type
    write(
      const byte_buffer_ref buffer
      )
    {
      return write(buffer.get_buffer(), buffer.get_size());
    }
};

}
