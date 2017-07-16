#pragma once
#include "tcp_socket.h"
#include "detail/ssl_context.h"

#include <mini/io/stream.h>

namespace mini::net {

class ssl_stream
  : public io::stream
{
  public:
    ssl_stream(
      io::stream& underlying_stream,
      const string_ref target_name = string_ref::empty
      );

    ~ssl_stream(
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
    handshake(
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

    io::stream&
    get_underlying_stream(
      void
      );

    bool
    is_handshake_established(
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

    io::stream& _underlying_stream;
    ssl_context _context;
};

}
