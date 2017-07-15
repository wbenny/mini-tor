#include "ssl_socket.h"

namespace mini::net {

ssl_socket::ssl_socket(
  void
  )
{
  _context.initialize(_socket);
}

ssl_socket::~ssl_socket(
  void
  )
{
  close();
}

ssl_socket::ssl_socket(
  const string_ref host,
  uint16_t port
  )
  : ssl_socket()
{
  connect(host, port);
}

void
ssl_socket::close(
  void
  )
{
  _context.disconnect();
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
    _context.handshake() == SEC_E_OK;
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
    _context.is_valid();
}

size_type
ssl_socket::read_impl(
  void* buffer,
  size_type size
  )
{
  mutable_byte_buffer_ref buf(
    static_cast<byte_type*>(buffer),
    static_cast<byte_type*>(buffer) + size);

  return _context.read(buf);
}

size_type
ssl_socket::write_impl(
  const void* buffer,
  size_type size
  )
{
  byte_buffer_ref buf(
    static_cast<const byte_type*>(buffer),
    static_cast<const byte_type*>(buffer) + size);

  return _context.write(buf);
}

}
