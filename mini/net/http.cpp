#include "http.h"

#include "tcp_socket.h"
#include <mini/io/stream_reader.h>

namespace mini::net::http::client {

string
get_on_stream(
  io::stream& sock,
  const string_ref host,
  uint16_t port,
  const string_ref path
  )
{
  MINI_UNREFERENCED(port);

  string req = string::format("GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path.get_buffer(), host.get_buffer());
  sock.write(req);

  io::stream_reader sr(sock);
  string result = sr.read_string_to_end();

  auto header_position = result.index_of("\r\n\r\n");
  if (header_position != mini::string::not_found)
  {
    header_position += 4;
    result = result.substring(header_position);
  }

  return result;
}

string
get(
  const string_ref host,
  uint16_t port,
  const string_ref path
  )
{
  tcp_socket sock(host, port);

  if (sock.is_connected())
  {
    return get_on_stream(sock, host, port, path);
  }

  return string();
}

}
