#pragma once
#include "stream.h"

#include <mini/byte_buffer.h>

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
      read(result);

      return result;
    }

    template <
      typename T,
      typename = std::enable_if_t<std::is_pod_v<T>>
    >
    size_type
    read(
      T& result
      )
    {
      size_type bytes_read = read_impl(&result, sizeof(result));

      if (_endianness != current_endianness)
      {
        result = swap_endianness(result);
      }

      return bytes_read;
    }

    template <
      typename T,
      size_type N,
      typename = std::enable_if_t<std::is_pod_v<T>>
    >
    size_type
    read(
      T (&result)[N]
      )
    {
      return read(result, N * sizeof(T));
    }

    size_type
    read(
      void* buffer,
      size_type size
      )
    {
      return read_impl(buffer, size);
    }

    size_type
    read(
      mutable_byte_buffer_ref buffer
      )
    {
      return read_impl(buffer.get_buffer(), buffer.get_size());
    }

    template <
      typename T,
      typename = std::enable_if_t<std::is_pod_v<T>>
    >
    size_type
    write(
      const T& value
      )
    {
      T value_to_write = value;

      if (_endianness != current_endianness)
      {
        value_to_write = swap_endianness(value);
      }

      return write_impl(&value_to_write, sizeof(value_to_write));
    }

    size_type
    write(
      const byte_buffer_ref buffer
      )
    {
      return write_impl(buffer.get_buffer(), buffer.get_size());
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
    size_type
    read_impl(
      void* buffer,
      size_type size
      )
    {
      byte_type* buffer_bytes = reinterpret_cast<byte_type*>(buffer);

      size_type total_bytes_read = 0;
      size_type bytes_read;
      while (stream::success(bytes_read = _stream.read(buffer_bytes + total_bytes_read, size - total_bytes_read)))
      {
        total_bytes_read += bytes_read;

        mini_break_if(total_bytes_read >= size);
      }

      mini_assert(stream::success(bytes_read) ? total_bytes_read == size : true);

      return total_bytes_read;
    }

    size_type
    write_impl(
      const void* buffer,
      size_type size
      )
    {
      const byte_type* buffer_bytes = reinterpret_cast<const byte_type*>(buffer);

      size_type total_bytes_written = 0;
      size_type bytes_written;
      while (stream::success(bytes_written = _stream.write(buffer_bytes + total_bytes_written, size - total_bytes_written)))
      {
        total_bytes_written += bytes_written;

        mini_break_if(total_bytes_written >= size);
      }

      mini_assert(stream::success(bytes_written) ? total_bytes_written == size : true);

      return total_bytes_written;
    }

    stream& _stream;
    endianness _endianness;
};

}
