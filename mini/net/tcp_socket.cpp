#include "tcp_socket.h"

namespace mini::net {

static WSADATA wsa_data = { 0 };

static void __cdecl
tcp_socket_global_init(
  void
  );

static void __cdecl
tcp_socket_global_destroy(
  void
  );

void __cdecl
tcp_socket_global_init(
  void
  )
{
  if (!wsa_data.wVersion)
  {
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
    atexit(tcp_socket_global_destroy);
  }
}

void __cdecl
tcp_socket_global_destroy(
  void
  )
{
  if (wsa_data.wVersion)
  {
    WSACleanup();
  }
}

//////////////////////////////////////////////////////////////////////////

tcp_socket::tcp_socket(
  void
  )
{
  tcp_socket_global_init();
}

tcp_socket::tcp_socket(
  const string_ref host,
  uint16_t port
  )
  : tcp_socket()
{
  connect(host, port);
}

tcp_socket::~tcp_socket(
  void
  )
{
  close();
}

bool
tcp_socket::connect(
  const string_ref host,
  uint16_t port
  )
{
  hostent* h = gethostbyname(host.get_buffer());

  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  memory::copy(&sin.sin_addr, h->h_addr_list[0], h->h_length);

  _ip = ip_address(sin.sin_addr.s_addr);

  if ((_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
  {
    return false;
  }

  if ((::connect(_socket, (sockaddr*)&sin, sizeof(sin))) == SOCKET_ERROR)
  {
    close();
    return false;
  }

  return true;
}

void
tcp_socket::close(
  void
  )
{
  closesocket(_socket);

  _socket = INVALID_SOCKET;
}

bool tcp_socket::can_read(
  void
  ) const
{
  return true;
}

bool tcp_socket::can_write(
  void
  ) const
{
  return true;
}

bool
tcp_socket::can_seek(
  void
  ) const
{
  return false;
}

size_type
tcp_socket::read(
  void* buffer,
  size_type size
  )
{
  //
  // early exit.
  //
  if (size == 0)
  {
    return 0;
  }

  size_type result = (size_type)recv(_socket, (char*)buffer, (int)size, 0);

  //
  // close & invalidate socket when we've received 0 bytes.
  //
  if (result == 0)
  {
    close();
  }

  //
  // invalidate socket when we've encountered an error.
  //
  if (result == SOCKET_ERROR)
  {
    _socket = INVALID_SOCKET;
  }

  return result;
}

size_type
tcp_socket::write(
  const void* buffer,
  size_type size
  )
{
  //
  // early exit.
  //
  if (size == 0)
  {
    return 0;
  }

  size_type result = (size_type)send(_socket, (const char*)buffer, (int)size, 0);

  //
  // invalidate socket when we've encountered an error.
  //
  if (result == SOCKET_ERROR)
  {
    _socket = INVALID_SOCKET;
  }

  return result;
}

size_type
tcp_socket::seek(
  intptr_t offset,
  seek_origin origin
  )
{
  MINI_UNREFERENCED(offset);
  MINI_UNREFERENCED(origin);

  return 0;
}

void
tcp_socket::flush(
  void
  )
{
  return;
}

size_type
tcp_socket::get_size(
  void
  ) const
{
  return 0;
}

size_type
tcp_socket::get_position(
  void
  ) const
{
  return 0;
}

}
