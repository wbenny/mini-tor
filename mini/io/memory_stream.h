#pragma once
#include "stream.h"

#include <mini/algorithm.h>

namespace mini::io {

class memory_stream
  : public stream
{
  public:
    memory_stream(
      void
      )
      : _begin(nullptr)
      , _end(nullptr)
      , _position(nullptr)
    {

    }

    memory_stream(
      const byte_buffer_ref buffer
      ) // : memory_stream()
    {
      attach(buffer);
    }

    void
    close(
      void
      ) override
    {

    }

    bool
    can_read(
      void
      ) const override
    {
      return true;
    }

    bool
    can_write(
      void
      ) const override
    {
      return true;
    }

    bool
    can_seek(
      void
      ) const override
    {
      return true;
    }

    size_type
    seek(
      intptr_t offset,
      seek_origin origin = seek_origin::current
      ) override
    {
      uint8_t* origin_pointer = origin == seek_origin::begin
                  ? _begin    : origin == seek_origin::current
                  ? _position : origin == seek_origin::end
                  ? _end      : nullptr;

      _position = algorithm::clamp(origin_pointer + offset, _begin, _end);

      return _position - _begin;
    }

    void
    flush(
      void
      ) override
    {
      return;
    }

    void
    attach(
      const byte_buffer_ref buffer
      )
    {
      _begin    = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(buffer.begin()));
      _end      = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(buffer.end()));
      _position = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(buffer.begin()));
    }

    size_type
    get_size(
      void
      ) const override
    {
      return _end - _begin;
    }

    size_type
    get_position(
      void
      ) const override
    {
      return _position - _begin;
    }

    void*
    get_buffer(
      void
      )
    {
      return _begin;
    }

    const void*
    get_buffer(
      void
      ) const
    {
      return _begin;
    }

  private:
    size_type
    read_impl(
      void* buffer,
      size_type size
      ) override
    {
      size = algorithm::min(size, static_cast<size_type>(_end - _position));
      memory::copy(buffer, _position, size);
      _position += size;
      return size;
    }

    size_type
    write_impl(
      const void* buffer,
      size_type size
      ) override
    {
      size = algorithm::min(size, static_cast<size_type>(_end - _position));
      memory::copy(_position, buffer, size);
      _position += size;
      return size;
    }

    byte_type* _begin;
    byte_type* _end;
    byte_type* _position;
};

}
