#include "tcp_socket.h"

namespace mini::net {

static WSADATA g_wsa_data = { 0 };

void
tcp_socket::global_init(
  void
  )
{
  if (!g_wsa_data.wVersion)
  {
    WSAStartup(MAKEWORD(2, 2), &g_wsa_data);
  }
}

void
tcp_socket::global_destroy(
  void
  )
{
  if (g_wsa_data.wVersion)
  {
    WSACleanup();
  }
}

tcp_socket::tcp_socket(
  void
  )
  : _socket(INVALID_SOCKET)
  , _port(0)
{
  global_init();
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

void
tcp_socket::connect(
  const string_ref host,
  uint16_t port
  )
{
  ::hostent* h = ::gethostbyname(host.get_buffer());

  ::sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = ::htons(port);
  memcpy(&sin.sin_addr, h->h_addr_list[0], h->h_length);

  _ip = ip_address(sin.sin_addr.s_addr);

  if ((_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == SOCKET_ERROR)
  {
    return;
  }

  if ((::connect(_socket, (::sockaddr*)&sin, sizeof(sin))) == SOCKET_ERROR)
  {
    ::closesocket(_socket);
    return;
  }

  return;
}

void
tcp_socket::close(
  void
  )
{
  ::closesocket(_socket);

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

bool tcp_socket::can_seek(
  void
  ) const
{
  return false;
}

size_t
tcp_socket::read(
  void* buffer,
  size_t size
  )
{
  return (size_t)::recv(_socket, (char*)buffer, (int)size, 0);
}

size_t
tcp_socket::write(
  const void* buffer,
  size_t size
  )
{
  return (size_t)::send(_socket, (const char*)buffer, (int)size, 0);
}

size_t
tcp_socket::seek(
  intptr_t offset,
  seek_origin origin
  )
{
  return 0;
}

void
tcp_socket::flush(
  void
  )
{
  return;
}

size_t
tcp_socket::get_size(
  void
  ) const
{
  return 0;
}

size_t
tcp_socket::get_position(
  void
  ) const
{
  return 0;
}

}
