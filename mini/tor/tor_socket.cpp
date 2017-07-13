#include "tor_socket.h"
#include "circuit.h"

#include <mini/logger.h>
#include <mini/io/memory_stream.h>
#include <mini/io/stream_wrapper.h>

namespace mini::tor {

tor_socket::tor_socket(
  onion_router* onion_router
  )
  : _onion_router(onion_router)
{
  if (onion_router != nullptr)
  {
    connect(onion_router);
  }
}

tor_socket::~tor_socket(
  void
  )
{
  close();
}

void
tor_socket::connect(
  onion_router* router
  )
{
  //
  // if this socket is alive, we need to close it first.
  //
  if (is_connected())
  {
    close();
  }

  set_state(state::connecting);

  _onion_router = router;

  _socket.reset(new net::ssl_socket(
    _onion_router->get_ip_address().to_string().get_buffer(),
    _onion_router->get_or_port()));

  if (!is_connected())
  {
    set_state(state::closed);
    return;
  }

  set_state(handshake_in_progress);

  //
  // handshake.
  //
  send_versions();
  recv_versions();

#if !defined(MINI_TOR_ASSUME_PROTOCOL_VERSION_PREFERRED)

  if (_protocol_version != protocol_version_preferred)
  {
    set_state(state::closed);
    return;
  }

#endif

  recv_certificates();
  recv_net_info();
  send_net_info();

  //
  // start the receive loop.
  //
  _recv_cell_loop_thread.reset(new threading::thread_function(
    [this]() { recv_cell_loop(); }));

  _recv_cell_loop_thread->start();

  //
  // this shouldn't fail unless the creation of the thread fails.
  //
  wait_for_state(state::ready);
}

void
tor_socket::close(
  void
  )
{
  if (get_state() == state::closing)
  {
    wait_for_state(state::closed);
    return;
  }

  if (get_state() == state::closed)
  {
    return;
  }

  set_state(state::closing);

  while (!_circuit_map.is_empty())
  {
    _circuit_map.end()[-1].second->send_destroy_cell();

    //
    // this call will:
    //   close all the streams in the circuit
    //   remove the circuit from our circuit map.
    //
    _circuit_map.end()[-1].second->destroy();
  }

  mini_assert(_circuit_map.is_empty());

  //
  // joins the receive loop thread.
  //
  set_state(state::closed);
}

circuit*
tor_socket::create_circuit(
  handshake_type handshake
  )
{
  if (!is_ready())
  {
    return nullptr;
  }

  circuit* new_circuit = new circuit(*this);
  _circuit_map.insert(new_circuit->get_circuit_id(), new_circuit);
  new_circuit->create(_onion_router, handshake);

  //
  // should't we close the socket in the case of failure?
  //
  return new_circuit->get_circuit_node_list_size() == 1
    ? new_circuit
    : nullptr;
}

void
tor_socket::remove_circuit(
  circuit* circuit
  )
{
  mini_debug("tor_socket::remove_circuit() [circuit: %u]", circuit->get_circuit_id() & 0x7FFFFFFF);

  _circuit_map.remove(circuit->get_circuit_id());
}

void
tor_socket::send_cell(
  const cell& cell
  )
{
  if (is_connected())
  {
    byte_buffer cell_content = cell.get_bytes(static_cast<protocol_version_type>(_protocol_version));
    _socket->write(cell_content.get_buffer(), cell_content.get_size());
  }
}

cell
tor_socket::recv_cell(
  void
  )
{
  cell cell;

  if (is_connected()) do
  {
    io::stream_wrapper socket_buffer(*_socket, endianness::big_endian);

    //
    // get circuit id based on the current protocol version.
    //
    circuit_id_type circuit_id;
    if (_protocol_version < 4)
    {
      circuit_id_v3_type circuit_id_v3;
      mini_break_if(socket_buffer.read(circuit_id_v3) != sizeof(circuit_id_v3_type));

      circuit_id = static_cast<circuit_id_type>(circuit_id_v3);
    }
    else
    {
      mini_break_if(socket_buffer.read(circuit_id) != sizeof(circuit_id_type));
    }

    //
    // get the cell command.
    //
    cell_command command;
    mini_break_if(socket_buffer.read(command) != sizeof(cell_command));

    //
    // get payload size for variable-length cell types.
    //
    payload_size_type payload_size = cell::payload_size;
    if (cell::is_variable_length_cell_command(command))
    {
      mini_break_if(socket_buffer.read<payload_size_type>(payload_size) != sizeof(payload_size_type));
    }

    //
    // get the content of the payload.
    //
    byte_buffer payload(payload_size);
    mini_break_if(socket_buffer.read(payload.get_buffer(), payload_size) != payload_size);

    //
    // build the cell
    //
    cell.set_circuit_id(circuit_id);
    cell.set_command(command);
    cell.set_payload(payload);
    cell.mark_as_valid();
  } while (false);

  return cell;
}

protocol_version_type
tor_socket::get_protocol_version(
  void
  ) const
{
  return static_cast<protocol_version_type>(_protocol_version);
}

onion_router*
tor_socket::get_onion_router(
  void
  )
{
  return _onion_router;
}

circuit*
tor_socket::get_circuit_by_id(
  circuit_id_type circuit_id
  )
{
  circuit** circuit = _circuit_map.find(circuit_id);

  return circuit
    ? *circuit
    : nullptr;
}

bool
tor_socket::is_connected(
  void
  ) const
{
  return
    _socket &&
    _socket->is_connected();
}

bool
tor_socket::is_ready(
  void
  ) const
{
  return
    is_connected() &&
    get_state() == state::ready;
}

tor_socket::state
tor_socket::get_state(
  void
  ) const
{
  return _state.get_value();
}

void
tor_socket::set_state(
  state new_state
  )
{
  if (new_state == state::closed)
  {
    //
    // close the socket and wait for the thread to end.
    // this must be done before the actual change
    // of the state.
    //
    _socket.reset();
    _recv_cell_loop_thread->join();

    //
    // terminate the thread.
    //
    _recv_cell_loop_thread.reset();

    //
    // set back the protocol version to 3.
    //
    _protocol_version = protocol_version_initial;
  }

  _state.set_value(new_state);
}

threading::wait_result
tor_socket::wait_for_state(
  state desired_state,
  timeout_type timeout
  )
{
  return _state.wait_for_value(desired_state, timeout);
}

void
tor_socket::send_versions(
  void
  )
{
  mini_debug("tor_socket::send_versions()");

  // static constexpr protocol_version_type supported_versions[] = { 4 };
  send_cell(cell(0, cell_command::versions, { 0, 4 }));
}

void
tor_socket::recv_versions(
  void
  )
{
  mini_debug("tor_socket::recv_versions()");

  cell versions_cell = recv_cell();

#if !defined(MINI_TOR_ASSUME_PROTOCOL_VERSION_PREFERRED)

  io::memory_stream versions_stream(versions_cell.get_payload());
  io::stream_wrapper versions_buffer(versions_stream, endianness::big_endian);

  for (
    size_type i = 0;
    i < versions_cell.get_payload().get_size();
    i += sizeof(protocol_version_type)
    )
  {
    protocol_version_type offered_version = versions_buffer.read<protocol_version_type>();

    if (offered_version == protocol_version_preferred)
    {
      _protocol_version = offered_version;
      break;
    }
  }

#else

  _protocol_version = protocol_version_preferred;

#endif
}

void
tor_socket::send_net_info(
  void
  )
{
  mini_debug("tor_socket::send_net_info()");

  const uint32_t remote = _socket->get_underlying_socket().get_ip().to_int();
  const uint32_t local = 0; // FIXME: local IP address.
  const uint32_t epoch = time::now().to_timestamp();

  byte_buffer net_info_bytes(4 + 2 + 4 + 3 + 4);
  io::memory_stream net_info_stream(net_info_bytes);
  io::stream_wrapper net_info_buffer(net_info_stream, endianness::big_endian);

  //
  // If version 2 or higher is negotiated, each party sends the other a
  // NETINFO cell.  The cell's payload is:
  //
  //  Timestamp              [4 bytes]
  //  Other OR's address     [variable]
  //  Number of addresses    [1 byte]
  //  This OR's addresses    [variable]
  //
  // Address is:
  //   Type   (1 octet)
  //   Length (1 octet)
  //   Value  (variable-width)
  //
  //  "Type" is one of:
  //    0x00 -- Hostname
  //    0x04 -- IPv4 address
  //    0x06 -- IPv6 address
  //    0xF0 -- Error, transient
  //    0xF1 -- Error, nontransient
  //

  net_info_buffer.write(epoch);
  net_info_buffer.write(static_cast<byte_type>(0x04)); // type
  net_info_buffer.write(static_cast<byte_type>(0x04)); // length
  net_info_buffer.write(swap_endianness(remote));
  net_info_buffer.write(static_cast<byte_type>(0x01)); // number of addresses
  net_info_buffer.write(static_cast<byte_type>(0x04)); // type
  net_info_buffer.write(static_cast<byte_type>(0x04)); // length
  net_info_buffer.write(swap_endianness(local));

  send_cell(cell(
    0,
    cell_command::netinfo,
    net_info_bytes));
}

void
tor_socket::recv_net_info(
  void
  )
{
  recv_cell(); // netinfo

  mini_debug("tor_socket::recv_net_info()");
}

void
tor_socket::send_certificates(
  void
  )
{

}

void
tor_socket::recv_certificates(
  void
  )
{
  recv_cell(); // certs
  recv_cell(); // auth_challenge

  mini_debug("tor_socket::recv_certificates()");
}

void
tor_socket::recv_cell_loop(
  void
  )
{
  set_state(state::ready);

  for (;;)
  {
    cell cell = recv_cell();

    if (get_state() == state::closing)
    {
      //
      // probably end of the stream.
      //
      break;
    }

    if (cell.is_valid() == false)
    {
      mini_warning("tor_socket::recv_cell_loop() !! received invalid cell, closing stream");
      close();
      break;
    }

    if (circuit* circuit = get_circuit_by_id(cell.get_circuit_id()))
    {
      circuit->handle_cell(cell);
    }
    else
    {
      mini_warning(
        "tor_socket::recv_cell_loop() !! received cell for non-existent circuit-id: %u",
        cell.get_circuit_id() & 0x7fffffff);
    }
  }
}

}
