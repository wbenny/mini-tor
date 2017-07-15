#pragma once
#include "tcp_socket.h"
#include "detail/ssl_context.h"

#include <mini/io/stream.h>

namespace mini::net {

class ssl_socket
  : public io::stream
{
  public:
    ssl_socket(
      void
      );

    ssl_socket(
      const string_ref host,
      uint16_t port
      );

    ~ssl_socket(
      void
      );

    void
    close(
      void
      ) override;

    bool
    can_read(
      void
      ) const override;

    bool
    can_write(
      void
      ) const override;

    bool
    can_seek(
      void
      ) const override;

    bool
    connect(
      const string_ref host,
      uint16_t port
      );

    size_type
    seek(
      intptr_t offset,
      seek_origin origin = seek_origin::current
      ) override;

    void
    flush(
      void
      ) override;

    size_type
    get_size(
      void
      ) const override;

    size_type
    get_position(
      void
      ) const override;

    tcp_socket&
    get_underlying_socket(
      void
      );

    bool
    is_connected(
      void
      ) const;

  private:
    size_type
    read_impl(
      void* buffer,
      size_type size
      ) override;

    size_type
    write_impl(
      const void* buffer,
      size_type size
      ) override;

    using ssl_context = detail::ssl_context;

    tcp_socket  _socket;
    ssl_context _context;
};

}
