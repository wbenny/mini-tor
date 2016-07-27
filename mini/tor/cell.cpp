#include "cell.h"

#include <mini/io/memory_stream.h>
#include <mini/io/stream_wrapper.h>

namespace mini::tor {

cell::cell(
  cell&& other
  )
{
  swap(other);
}

cell::cell(
  circuit_id_type circuit_id,
  cell_command command,
  const byte_buffer_ref payload
  )
  : _circuit_id(circuit_id)
  , _command(command)
  , _payload(payload)
{

}

void
cell::swap(
  cell& other
  )
{
  mini::swap(_circuit_id, other._circuit_id);
  mini::swap(_command, other._command);
  mini::swap(_payload, other._payload);
}

circuit_id_type
cell::get_circuit_id(
  void
  ) const
{
  return _circuit_id;
}

void
cell::set_circuit_id(
  circuit_id_type circuit_id
  )
{
  _circuit_id = circuit_id;
}

cell_command
cell::get_command(
  void
  ) const
{
  return _command;
}

void
cell::set_command(
  cell_command command
  )
{
  _command = command;
}

byte_buffer_ref
cell::get_payload(
  void
  ) const
{
  return _payload;
}

void
cell::set_payload(
  const byte_buffer_ref payload
  )
{
  _payload = payload;
}

byte_buffer
cell::get_bytes(
  protocol_version_type protocol_version
  ) const
{
  byte_buffer cell_bytes;

  if (_command == cell_command::versions || (uint32_t)_command >= 128)
  {
    cell_bytes.resize(
      //
      // circuit id.
      //
      (protocol_version < 4 ? sizeof(circuit_id_v3_type) : sizeof(circuit_id_type)) +

      //
      // cell command.
      //
      sizeof(cell_command) +

      //
      // payload size (16 bits).
      //
      sizeof(payload_size_type) +

      //
      // payload.
      //
      _payload.get_size());
  }
  else
  {
    cell_bytes.resize(514);
  }

  io::memory_stream cell_stream(cell_bytes);
  io::stream_wrapper cell_buffer(cell_stream, endianness::big_endian);

  //
  // tor-spec.txt
  // 5.1.1.
  //
  // In link protocol 3 or lower, CircIDs are 2 bytes long;
  // in protocol 4 or higher, CircIDs are 4 bytes long.
  //

  if (protocol_version < 4)
  {
    cell_buffer.write<circuit_id_v3_type>(_circuit_id);
  }
  else
  {
    cell_buffer.write(_circuit_id);
  }

  cell_buffer.write(_command);

  if (_command == cell_command::versions || (uint32_t)_command >= 128)
  {
    cell_buffer.write(static_cast<payload_size_type>(_payload.get_size()));
  }

  cell_buffer.write(_payload);

  return cell_bytes;
}

bool
cell::is_recognized(
  void
  ) const
{
  return _payload[1] == 0
      && _payload[2] == 0;
}

bool
cell::is_valid(
  void
  ) const
{
  return _is_valid;
}

void
cell::mark_as_valid(
  void
  )
{
  _is_valid = true;
}

}
