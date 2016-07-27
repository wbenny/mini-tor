#pragma once
#include "relay_cell.h"
#include "circuit_node.h"
#include "circuit.h"

#include <mini/io/memory_stream.h>
#include <mini/io/stream_wrapper.h>

namespace mini::tor {

relay_cell::relay_cell(
  circuit_node* node,
  const cell& cell
  )
  : cell(cell)
  , _circuit_node(node)
{
  io::memory_stream  payload_stream(cell.get_payload());
  io::stream_wrapper payload_buffer(payload_stream, endianness::big_endian);

  cell_command       relay_command = payload_buffer.read<cell_command>();
  uint16_t           dummy         = payload_buffer.read<uint16_t>();
  tor_stream_id_type stream_id     = payload_buffer.read<tor_stream_id_type>();
  uint32_t           digest        = payload_buffer.read<uint32_t>();
  payload_size_type  payload_size  = payload_buffer.read<payload_size_type>();

  byte_buffer payload(payload_size);
  payload_buffer.read(payload);

  _relay_command = relay_command;
  _stream_id = stream_id;
  this->set_relay_payload(payload);
}

relay_cell::relay_cell(
  circuit_id_type circuit_id,
  cell_command command,
  circuit_node* node,
  cell_command relay_command,
  tor_stream_id_type stream_id,
  const byte_buffer_ref relay_payload
  )
  : cell(circuit_id, command)
  , _circuit_node(node)
  , _relay_command(relay_command)
  , _stream_id(stream_id)
{
  this->set_relay_payload(relay_payload);
}

tor_stream_id_type
relay_cell::get_stream_id(
  void
  ) const
{
  return _stream_id;
}

tor_stream*
relay_cell::get_stream(
  void
  )
{
  return _circuit_node->get_circuit()->get_stream_by_id(_stream_id);
}

cell_command
relay_cell::get_relay_command(
  void
  ) const
{
  return _relay_command;
}

circuit_node*
relay_cell::get_circuit_node(
  void
  )
{
  return _circuit_node;
}

void
relay_cell::set_digest(
  const byte_buffer_ref digest
  )
{
  memory::copy(_digest, digest.get_buffer(), sizeof(_digest));
}

byte_buffer_ref
relay_cell::get_relay_payload(
  void
  ) const
{
  return _relay_payload;
}

void
relay_cell::set_relay_payload(
  const byte_buffer_ref payload
  )
{
  _relay_payload = payload;
}

bool
relay_cell::is_relay_cell_valid(
  void
  ) const
{
  //
  // each valid relay cell has set its circuit node.
  //
  return _circuit_node != nullptr;
}

}
