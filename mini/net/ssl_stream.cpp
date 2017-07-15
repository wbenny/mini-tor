#include "ssl_stream.h"

namespace mini::net {

ssl_stream::ssl_stream(
  io::stream& underlying_stream
  )
  : _underlying_stream(underlying_stream)
{
  _context.initialize(_underlying_stream);
}

ssl_stream::~ssl_stream(
  void
  )
{
  close();
}

void
ssl_stream::close(
  void
  )
{
  _context.disconnect();
}

bool
ssl_stream::can_read(
  void
  ) const
{
  return true;
}

bool
ssl_stream::can_write(
  void
  ) const
{
  return true;
}

bool
ssl_stream::can_seek(
  void
  ) const
{
  return false;
}

bool
ssl_stream::handshake(
  const string_ref host,
  uint16_t port
  )
{
  MINI_UNREFERENCED(host);
  MINI_UNREFERENCED(port);

  return _context.handshake() == SEC_E_OK;
}

size_type
ssl_stream::seek(
  intptr_t offset,
  seek_origin origin
  )
{
  MINI_UNREFERENCED(offset);
  MINI_UNREFERENCED(origin);

  return 0;
}

void
ssl_stream::flush()
{

}

size_type
ssl_stream::get_size(
  void
  ) const
{
  return 0;
}

size_type
ssl_stream::get_position(
  void
  ) const
{
  return 0;
}

io::stream&
ssl_stream::get_underlying_stream(
  void
  )
{
  return _underlying_stream;
}

bool
ssl_stream::is_handshake_established(
  void
  ) const
{
  return _context.is_valid();
}

size_type
ssl_stream::read_impl(
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
ssl_stream::write_impl(
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
