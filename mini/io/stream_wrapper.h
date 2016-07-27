#pragma once
#include "stream.h"

#include <mini/byte_buffer.h>

namespace mini {

enum class endianness
{
  little_endian,
  big_endian,
};

static constexpr endianness current_endianness = endianness::little_endian;

}

namespace mini::io {

class stream_wrapper
{
  public:
    stream_wrapper(
      stream& stream,
      endianness endianness = current_endianness
      )
      : _stream(stream)
      , _endianness(endianness)
    {

    }

    template <
      typename T,
      typename = std::enable_if_t<std::is_pod_v<T>>
    >
    T
    read(
      void
      )
    {
      T result = T();
      size_t bytes_read = read(result);

      return result;
    }

    template <
      typename T,
      typename = std::enable_if_t<std::is_pod_v<T>>
    >
    size_t
    read(
      T& result
      )
    {
      size_t bytes_read = _stream.read(&result, sizeof(result));

      if (_endianness != current_endianness)
      {
        result = swap_endianness(result);
      }

      return bytes_read;
    }

    template <
      typename T,
      size_t N,
      typename = std::enable_if_t<std::is_pod_v<T>>
    >
    size_t
    read(
      T (&result)[N]
      )
    {
      return read(result, N * sizeof(T));
    }

    size_t
    read(
      void* buffer,
      size_t size
      )
    {
      return _stream.read(buffer, size);
    }

    size_t
    read(
      mutable_byte_buffer_ref buffer
      )
    {
      return _stream.read(buffer.get_buffer(), buffer.get_size());
    }

    template <
      typename T,
      typename = std::enable_if_t<std::is_pod_v<T>>
    >
    size_t
    write(
      const T& value
      )
    {
      T value_to_write = value;

      if (_endianness != current_endianness)
      {
        value_to_write = swap_endianness(value);
      }

      return _stream.write(&value_to_write, sizeof(value_to_write));
    }

    size_t
    write(
      const byte_buffer_ref buffer
      )
    {
      return _stream.write(buffer.get_buffer(), buffer.get_size());
    }

    stream&
    get_stream(
      void
      )
    {
      return _stream;
    }

    const stream&
    get_stream(
      void
      ) const
    {
      return _stream;
    }

    bool
    is_end_of_stream(
      void
      ) const
    {
      return _stream.get_position() == _stream.get_size();
    }

  private:
    stream& _stream;
    endianness _endianness;
};

}
