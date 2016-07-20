#include "socket.h"

#include <mini/time.h>

namespace mini { namespace tor {

void
socket::connect(
  net::ip& ip,
  uint16_t port)
{
  _socket.connect(ip.to_string().get_buffer(), port);
  send_cell(cell(0, cell::command_type::versions, { 0, 3, 0, 4 }));
  cell version_reply = recv_cell();

  for (size_t i = 0; i < version_reply.payload.get_size(); i += 2)
  {
    uint16_t offered_version = swap_endianness(*(uint16_t*)(version_reply.payload.get_buffer() + i));
    if (offered_version <= protocol_version_max && offered_version > _protocol_version)
    {
      _protocol_version = offered_version;
    }
  }

//  std::thread t(this, &socket::receive_handler_loop);
}

void
socket::send_cell(
  const cell& c)
{
  printf(">> send_cell [circ_id: %i, cmd_id: %i]\n", c.circuit_id, c.command);
  auto cell_content = c.get_bytes(_protocol_version);
  _socket.write(cell_content.get_buffer(), cell_content.get_size());
}

cell
socket::recv_cell()
{
  cell result;

  uint8_t header[5];
  _socket.read(header, _protocol_version == 3 ? 3 : 5);

  if (_protocol_version < 4)
  {
    result.circuit_id = (uint32_t)swap_endianness(*(uint16_t*)header);
  }
  else
  {
    result.circuit_id = (uint32_t)swap_endianness(*(uint32_t*)header);
  }

  result.command = (cell::command_type)header[_protocol_version < 4 ? 2 : 4];

  size_t payload_length = 509;
  if (result.command == cell::command_type::versions || (uint32_t)result.command >= 128)
  {
    _socket.read((uint8_t*)&payload_length, 2);
    payload_length = swap_endianness((uint16_t)payload_length);
  }

  result.payload.resize(payload_length);
  _socket.read(result.payload.get_buffer(), payload_length);

  printf("<< recv_cell [circ_id: %i, cmd_id: %u]\n", result.circuit_id, result.command);

  return result;
}

void
socket::send_net_info()
{
  byte_buffer nibuf(4 + 2 + 4 + 3 + 4);
  uint32_t remote = _socket.get_underlying_socket().get_ip().to_int();
  uint32_t local = 0xC0A80016;
  uint32_t epoch = time::now().to_timestamp();

  *(uint32_t*)&nibuf[0] = swap_endianness(epoch);
  *(uint8_t*)&nibuf[4] = 0x04;
  *(uint8_t*)&nibuf[5] = 0x04;
  *(uint32_t*)&nibuf[6] = swap_endianness(remote);
  *(uint8_t*)&nibuf[10] = 0x01;
  *(uint8_t*)&nibuf[11] = 0x04;
  *(uint8_t*)&nibuf[12] = 0x04;
  *(uint32_t*)&nibuf[13] = swap_endianness(local);
  send_cell(cell(0, cell::command_type::netinfo, nibuf));
}

void
socket::recv_certificates(
  const cell& c)
{
  size_t certificate_count = c.payload[0];
}

void
socket::receive_handler_loop()
{
  for (;;)
  {
    cell c = recv_cell();

    switch (c.command)
    {
      case cell::command_type::netinfo:
        send_net_info();
        set_state(state::ready);
        break;

      case cell::command_type::certs:
        recv_certificates(c);
        break;

      default:
        //circuit circ = 

        break;
    }

  }
}

void
socket::set_state(
  state new_state)
{
  _state = new_state;
}

string
socket::fetch_hs_descriptor(const string& onion)
{
  return string();
}


} }
