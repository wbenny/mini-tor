#pragma once
#include <mini/string.h>
#include <mini/net/ip_address.h>
#include <mini/io/stream.h>

#include <winsock2.h>

namespace mini::net {

class tcp_socket
  : public io::stream
{
  public:
    tcp_socket(
      void
      );

    tcp_socket(
      const string_ref host,
      uint16_t port
      );

    ~tcp_socket(
      void
      );

    const char*
    get_host(
      void
      ) const
    {
      return _host.get_buffer();
    }

    const ip_address&
    get_ip(
      void
      ) const
    {
      return _ip;
    }

    uint16_t
    get_port(
      void
      ) const
    {
      return _port;
    }

    bool
    connect(
      const string_ref host,
      uint16_t port
      );

    void
    close(
      void
      );

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

    size_type
    read(
      void* buffer,
      size_type size
      ) override;

    size_type
    write(
      const void* buffer,
      size_type size
      ) override;

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

    bool
    is_connected(
      void
      ) const
    {
      return _socket != INVALID_SOCKET;
    }

  private:
    string _host;
    ip_address _ip;

    SOCKET _socket = INVALID_SOCKET;
    uint16_t _port = 0;
};

}
