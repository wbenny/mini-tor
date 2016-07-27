#pragma once
#include <mini/common.h>

namespace mini::io {

class stream
{
  public:
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
