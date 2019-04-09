#include "ssl_socket.h"

namespace mini::net {

ssl_socket::ssl_socket(
  const string_ref host
  )
  : _ssl_stream(_socket, host)
{

}

ssl_socket::ssl_socket(
  const string_ref host,
  uint16_t port
  )
  : ssl_socket(host)
{
  connect(host, port);
}

ssl_socket::~ssl_socket(
  void
  )
{
  close();
}

void
ssl_socket::close(
  void
  )
{
  _ssl_stream.close();
  _socket.close();
}

bool
ssl_socket::can_read(
  void
  ) const
{
  return true;
}

bool
ssl_socket::can_write(
  void
  ) const
{
  return true;
}

bool
ssl_socket::can_seek(
  void
  ) const
{
  return false;
}

bool
ssl_socket::connect(
  const string_ref host,
  uint16_t port
  )
{
  return
    _socket.connect(host, port) &&
    _ssl_stream.handshake(host, port);
}

size_type
ssl_socket::seek(
  intptr_t offset,
  seek_origin origin
  )
{
  MINI_UNREFERENCED(offset);
  MINI_UNREFERENCED(origin);

  return 0;
}

void
ssl_socket::flush()
{

}

size_type
ssl_socket::get_size(
  void
  ) const
{
  return 0;
}

size_type
ssl_socket::get_position(
  void
  ) const
{
  return 0;
}

tcp_socket&
ssl_socket::get_underlying_socket(
  void
  )
{
  return _socket;
}

bool
ssl_socket::is_connected(
  void
  ) const
{
  return
    _socket.is_connected() &&
    _ssl_stream.is_handshake_established();
}

size_type
ssl_socket::read_impl(
  void* buffer,
  size_type size
  )
{
  return _ssl_stream.read(buffer, size);
}

size_type
ssl_socket::write_impl(
  const void* buffer,
  size_type size
  )
{
  return _ssl_stream.write(buffer, size);
}

}
