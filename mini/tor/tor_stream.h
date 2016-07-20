#pragma once
#include "types.h"

#include <mini/byte_buffer.h>
#include <mini/io/stream.h>
#include <mini/threading/locked_value.h>

namespace mini::tor {

class circuit;

class tor_stream
  : public io::stream
{
  public:
    tor_stream(
      tor_stream_id_type stream_id,
      circuit* circuit
      );

    ~tor_stream(
      void
      );

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
      return false;
    }

    size_t
    read(
      void* buffer,
      size_t size
      ) override;

    size_t
    write(
      const void* buffer,
      size_t size
      ) override;

    size_t
    seek(
      intptr_t offset,
      seek_origin origin = seek_origin::current
      ) override;

    void
    flush(
      void
      ) override;

    size_t
    get_size(
      void
      ) const override;

    size_t
    get_position(
      void
      ) const override;

    tor_stream_id_type
    get_stream_id(
      void
      ) const;

    void close(
      void
      );

  private:
    friend class circuit;

    enum class state
    {
      none,
      connecting,
      ready,
      destroyed,
    };

    void
    append_to_recv_buffer(
      const byte_buffer_ref buffer
      );

    state
    get_state(
      void
      );

    void
    set_state(
      state new_state
      );

    void
    wait_for_state(
      state desired_state
      );

    //
    // flow control.
    //

    void
    decrement_package_window(
      void
      );

    void
    increment_package_window(
      void
      );

    void
    decrement_deliver_window(
      void
      );

    bool
    consider_sending_sendme(
      void
      );

    static constexpr size_t window_start = 500;
    static constexpr size_t window_increment = 50;
    static constexpr size_t window_max_unflushed = 10;

    tor_stream_id_type _stream_id;
    circuit* _circuit;

    size_t _deliver_window = window_start;
    size_t _package_window = window_start;
    threading::mutex _window_mutex;

    byte_buffer _buffer;
    threading::mutex _buffer_mutex;

    threading::locked_value<state> _state = state::connecting;
};

}
