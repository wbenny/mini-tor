#include "circuit.h"
#include "circuit_node.h"
#include "hidden_service.h"
#include "crypto/hybrid_encryption.h"

#include <mini/logger.h>
#include <mini/crypto/base16.h>
#include <mini/io/memory_stream.h>
#include <mini/io/stream_reader.h>
#include <mini/io/stream_wrapper.h>

namespace mini::tor {

circuit::circuit(
  tor_socket& tor_socket
  )
  : _tor_socket(tor_socket)
  , _circuit_id(get_next_circuit_id())
{
  _circuit_id |= 0x80000000;
}

circuit::~circuit(
  void
)
{
  send_destroy_cell();
  destroy();
}

tor_socket&
circuit::get_tor_socket(
  void
  )
{
  return _tor_socket;
}

circuit_id_type
circuit::get_circuit_id(
  void
  ) const
{
  return _circuit_id;
}

const circuit_node_list&
circuit::get_circuit_node_list(
  void
  ) const
{
  return _node_list;
}

size_type
circuit::get_circuit_node_list_size(
  void
  ) const
{
  return _node_list.get_size();
}

circuit_node*
circuit::get_final_circuit_node(
  void
  )
{
  return _node_list.top().get();
}

tor_stream*
circuit::create_stream(
  const string_ref host,
  uint16_t port
  )
{
  //
  // tor-spec.txt
  // 6.2.
  //
  // ADDRPORT [nul-terminated string]
  // FLAGS[4 bytes]
  //
  // ADDRPORT is made of ADDRESS | ':' | PORT | [00]
  //

  const string host_port = string::format("%s:%hi", host.get_buffer(), port);

  byte_buffer relay_data_bytes(host_port.get_size() + 1);
  io::memory_stream relay_data_stream(relay_data_bytes);
  io::stream_wrapper relay_data_buffer(relay_data_stream, endianness::big_endian);
  relay_data_buffer.write(host_port);

  //
  // send RELAY_BEGIN cell.
  //
  const tor_stream_id_type stream_id = get_next_stream_id();

  tor_stream* stream = new tor_stream(stream_id, this);
  _stream_map.insert(stream_id, stream);

  mini_debug("circuit::create_stream() [url: %s, stream: %u, status: creating]", host_port.get_buffer(), stream_id);
  set_state(state::connecting);
  {
    send_relay_cell(stream_id, cell_command::relay_begin, relay_data_bytes);
  }

  if (mini_wait_success(wait_for_state(state::ready)))
  {
    mini_debug("circuit::create_stream() [url: %s, stream: %u, status: created]", host_port.get_buffer(), stream_id);
  }
  else
  {
    mini_error("circuit::create_stream() [is_ready() == false]");
  }

  //
  // if the circuit has been destroyed,
  // the stream has been destroyed as well,
  // so we don't need to delete it here.
  //

  return is_ready()
    ? stream
    : nullptr;
}

tor_stream*
circuit::create_onion_stream(
  const string_ref onion,
  uint16_t port
  )
{
  hidden_service hidden_service_connector(this, onion);

  return hidden_service_connector.connect()
    ? create_stream(onion, port)
    : nullptr;
}

tor_stream*
circuit::create_dir_stream(
  void
  )
{
  const tor_stream_id_type stream_id = get_next_stream_id();

  tor_stream* stream = new tor_stream(stream_id, this);
  _stream_map.insert(stream_id, stream);

  mini_debug("circuit::create_dir_stream() [stream: %u, state: connecting]", stream_id);
  set_state(state::connecting);
  {
    send_relay_cell(stream_id, cell_command::relay_begin_dir);
  }

  if (mini_wait_success(wait_for_state(state::ready)))
  {
    mini_debug("circuit::create_dir_stream() [stream: %u, state: connected]", stream_id);
  }
  else
  {
    mini_error("circuit::create_dir_stream() [is_ready() == false]");
  }

  //
  // if the circuit has been destroyed,
  // the stream has been destroyed as well,
  // so we don't need to delete it here.
  //

  return is_ready()
    ? stream
    : nullptr;
}

void
circuit::create(
  onion_router* first_onion_router
  )
{
  mini_debug("circuit::create() [or: %s, state: creating]", first_onion_router->get_name().get_buffer());
  set_state(state::creating);
  {
    _extend_node = create_circuit_node(first_onion_router);

    send_cell(cell(
      _circuit_id,
      cell_command::create,
      _extend_node->create_onion_skin()
    ));
  }

  if (mini_wait_success(wait_for_state(state::ready)))
  {
    mini_debug("circuit::create() [or: %s, state: created]", first_onion_router->get_name().get_buffer());
  }
  else
  {
    mini_error("circuit::create() [or: %s, state: destroyed]", first_onion_router->get_name().get_buffer());
  }
}

void
circuit::extend(
  onion_router* next_onion_router
)
{
  mini_debug("circuit::extend() [or: %s, state: extending]", next_onion_router->get_name().get_buffer());
  set_state(state::extending);
  {
    _extend_node = create_circuit_node(next_onion_router);
    const byte_buffer onion_skin = _extend_node->create_onion_skin();

    byte_buffer relay_payload_bytes(
      4 +                         // ip address
      2 +                         // port
      onion_skin.get_size() +     // hybrid encrypted data length
      HASH_LEN);                  // identity fingerprint

    io::memory_stream relay_payload_stream(relay_payload_bytes);
    io::stream_wrapper relay_payload_buffer(relay_payload_stream, endianness::big_endian);

    relay_payload_buffer.write(swap_endianness(next_onion_router->get_ip_address().to_int()));
    relay_payload_buffer.write(next_onion_router->get_or_port());
    relay_payload_buffer.write(onion_skin);
    relay_payload_buffer.write(crypto::base16::decode(next_onion_router->get_identity_fingerprint()));

    send_relay_cell(
      0,
      cell_command::relay_extend,
      relay_payload_bytes,

      //
      // clients MUST only send
      // EXTEND cells inside RELAY_EARLY cells
      //

      cell_command::relay_early,
      _extend_node);
  }
  if (mini_wait_success(wait_for_state(state::ready)))
  {
    mini_debug("circuit::extend() [or: %s, state: extended]", next_onion_router->get_name().get_buffer());
  }
  else
  {
    mini_error("circuit::extend() [or: %s, state: destroyed]", next_onion_router->get_name().get_buffer());
  }
}

void
circuit::destroy(
  void
  )
{
  if (get_state() == state::destroyed)
  {
    return;
  }

  mini_debug("circuit::destroy()");

  if (_extend_node)
  {
    delete _extend_node;
    _extend_node = nullptr;
  }

  close_streams();
  _tor_socket.remove_circuit(this);
}

bool
circuit::is_destroyed(
  void
  ) const
{
  return get_state() == state::destroyed;
}

bool
circuit::is_ready(
  void
  ) const
{
  return get_state() == state::ready;
}

tor_stream*
circuit::get_stream_by_id(
  tor_stream_id_type stream_id
  )
{
  tor_stream** stream = _stream_map.find(stream_id);

  return stream
    ? *stream
    : nullptr;
}

void
circuit::close_streams(
  void
  )
{
  //
  // destroy each stream in this circuit.
  //
  while (!_stream_map.is_empty())
  {
    //
    // this call removes the stream from our stream map.
    //
    send_relay_end_cell(_stream_map.last_value());
  }

  _node_list.clear();

  //
  // signal destroy.
  //
  set_state(state::destroyed);
}

circuit_node*
circuit::create_circuit_node(
  onion_router* router,
  circuit_node_type type
  )
{
  return new circuit_node(this, router, type);
}

void
circuit::rendezvous_establish(
  const byte_buffer_ref rendezvous_cookie
  )
{
  mini_assert(rendezvous_cookie.get_size() == 20);

  mini_debug("circuit::rendezvous_establish() [circuit: %u, state: establishing]", _circuit_id);
  set_state(state::rendezvous_establishing);
  {
    send_relay_cell(
      0,
      cell_command::relay_command_establish_rendezvous,
      rendezvous_cookie);
  }

  if (mini_wait_success(wait_for_state(state::rendezvous_established)))
  {
    mini_debug("circuit::rendezvous_establish() [circuit: %u, state: established]", _circuit_id);
  }
  else
  {
    mini_error("circuit::rendezvous_establish() [circuit: %u, is_rendezvous_established() == false]", _circuit_id);
  }
}

bool
circuit::is_rendezvous_established(
  void
  ) const
{
  return get_state() == state::rendezvous_established;
}

void
circuit::rendezvous_introduce(
  circuit* rendezvous_circuit,
  const byte_buffer_ref rendezvous_cookie
  )
{
  mini_assert(rendezvous_cookie.get_size() == 20);

  auto introduction_point = get_final_circuit_node()->get_onion_router();
  onion_router* introducee = rendezvous_circuit->get_final_circuit_node()->get_onion_router();

  mini_debug("circuit::rendezvous_introduce() [or: %s, state: introducing]", introduction_point->get_name().get_buffer());
  set_state(state::rendezvous_introducing);

  mini_debug("circuit::rendezvous_introduce() [or: %s, state: completing]", introduction_point->get_name().get_buffer());
  rendezvous_circuit->set_state(state::rendezvous_completing);
  {
    //
    // payload of the RELAY_COMMAND_INTRODUCE1
    // command:
    //
    // PK_ID  Identifier for Bob's PK      [20 octets]
    // VER    Version byte: set to 2.        [1 octet]
    // IP     Rendezvous point's address    [4 octets]
    // PORT   Rendezvous point's OR port    [2 octets]
    // ID     Rendezvous point identity ID [20 octets]
    // KLEN   Length of onion key           [2 octets]
    // KEY    Rendezvous point onion key [KLEN octets]
    // RC     Rendezvous cookie            [20 octets]
    // g^x    Diffie-Hellman data, part 1 [128 octets]
    //

    //
    // compute PK_ID, aka hash of the service key.
    //
    auto service_key_hash = crypto::sha1::hash(introduction_point->get_service_key());

    //
    // create rest of the payload in separate buffer;
    // it will be encrypted.
    //
    byte_buffer handshake_bytes(
      1 +                                       // version
      4 +                                       // ip address
      2 +                                       // port
      HASH_LEN +                                // identity_fingerprint
      2 +                                       // onion key size
      introducee->get_onion_key().get_size() +  // onion key
      20 +                                      // rendezvous cookie
      128);                                     // DH

    io::memory_stream handshake_stream(handshake_bytes);
    io::stream_wrapper handshake_buffer(handshake_stream, endianness::big_endian);

    rendezvous_circuit->_extend_node = create_circuit_node(introduction_point, circuit_node_type::introduction_point);

    handshake_buffer.write(static_cast<uint8_t>(2));
    handshake_buffer.write(swap_endianness(introducee->get_ip_address().to_int()));
    handshake_buffer.write(introducee->get_or_port());
    handshake_buffer.write(crypto::base16::decode(introducee->get_identity_fingerprint()));
    handshake_buffer.write(static_cast<payload_size_type>(introducee->get_onion_key().get_size()));
    handshake_buffer.write(introducee->get_onion_key());
    handshake_buffer.write(rendezvous_cookie);
    handshake_buffer.write(rendezvous_circuit->_extend_node->get_key_agreement().get_public_key());

    auto handshake_encrypted = hybrid_encryption::encrypt(
      handshake_bytes,
      introduction_point->get_service_key());

    //
    // compose the final payload.
    //
    byte_buffer relay_payload_bytes;
    relay_payload_bytes.add_many(service_key_hash);
    relay_payload_bytes.add_many(handshake_encrypted);

    //
    // send the cell.
    //
    send_relay_cell(
      0,
      cell_command::relay_command_introduce1,
      relay_payload_bytes);
  }

  if (mini_wait_success(wait_for_state(state::rendezvous_introduced)))
  {
    mini_debug("circuit::rendezvous_introduce() [or: %s, state: introduced]", introduction_point->get_name().get_buffer());
  }
  else
  {
    mini_error("circuit::rendezvous_introduce() [or: %s, is_rendezvous_introduced() == false]", introduction_point->get_name().get_buffer());

    //
    // we cannot expect the rendezvous will be completed.
    //

    return;
  }

  if (mini_wait_success(rendezvous_circuit->wait_for_state(state::rendezvous_completed)))
  {
    mini_debug("circuit::rendezvous_introduce() [or: %s, state: completed]", introduction_point->get_name().get_buffer());
  }
  else
  {
    mini_error("circuit::rendezvous_introduce() [or: %s, is_rendezvous_completed() == false]", introduction_point->get_name().get_buffer());
  }
}

bool
circuit::is_rendezvous_introduced(
  void
  ) const
{
  return get_state() == state::rendezvous_introduced;
}

bool
circuit::is_rendezvous_completed(
  void
  ) const
{
  return get_state() == state::rendezvous_completed;
}

cell&
circuit::encrypt(
  relay_cell& cell
  )
{
  return encrypt(std::move(cell));
}

cell&
circuit::encrypt(
  relay_cell&& cell
  )
{
  for (int i = (int)_node_list.get_size() - 1; i >= 0; i--)
  {
    _node_list[i]->encrypt_forward_cell(cell);
  }

  return cell;
}

relay_cell
circuit::decrypt(
  cell& cell
  )
{
  for (auto&& node : _node_list)
  {
    if (node->decrypt_backward_cell(cell))
    {
      return relay_cell(node.get(), cell);
    }
  }

  return relay_cell();
}

void
circuit::send_cell(
  const cell& cell
  )
{
  _tor_socket.send_cell(cell);
}

void
circuit::send_destroy_cell(
  void
  )
{
  send_cell(cell(
    _circuit_id,
    cell_command::destroy,
    nullptr
  ));
}

void
circuit::send_relay_cell(
  tor_stream_id_type    stream_id,
  cell_command          relay_command,
  const byte_buffer_ref payload,
  cell_command          cell_command,
  circuit_node*         node
  )
{
  node = node ? node : get_final_circuit_node();

  if (get_stream_by_id(stream_id) == nullptr && stream_id != 0)
  {
    mini_warning("circuit::send_relay_cell() attempt to send cell to non-existent stream-id: %u", stream_id);
    return;
  }

  mini_debug(
    "tor_socket::send_cell() [circuit: %i%s, stream: %u, command: %i, relay_command: %i]",
    _circuit_id & 0x7FFFFFFF,
    (_circuit_id & 0x80000000 ? " (MSB set)" : ""),
    stream_id,
    cell_command,
    relay_command);

  send_cell(encrypt(relay_cell(
    _circuit_id,
    cell_command,
    node,
    relay_command,
    stream_id,
    payload)));
}

void
circuit::send_relay_data_cell(
  tor_stream* stream,
  const byte_buffer_ref buffer
  )
{
  for (size_type i = 0; i < round_up_to_multiple(buffer.get_size(), relay_cell::payload_data_size); i += relay_cell::payload_data_size)
  {
    const size_type data_size = min(buffer.get_size() - i, relay_cell::payload_data_size);

    get_final_circuit_node()->decrement_package_window();

    send_relay_cell(
      stream->get_stream_id(),
      cell_command::relay_data,
      buffer.slice(i, i + data_size));
  }
}

void
circuit::send_relay_end_cell(
  tor_stream* stream
  )
{
  send_relay_cell(
    stream->get_stream_id(),
    cell_command::relay_end,
    { 6 }); // reason

  //
  // signal destroy.
  //
  stream->set_state(tor_stream::state::destroyed);

  _stream_map.remove(stream->get_stream_id());
}

void
circuit::send_relay_sendme_cell(
  tor_stream* stream
  )
{
  //
  // if stream == nullptr, we're sending RELAY_SENDME
  // with stream_id = 0, which means circuit RELAY_SENDME
  //

  send_relay_cell(
    stream != nullptr ? stream->get_stream_id() : 0,
    cell_command::relay_sendme,
    nullptr);
}

void
circuit::handle_cell(
  cell& cell
  )
{
  if (cell.get_command() != cell_command::relay)
  {
    mini_debug(
      "tor_socket::recv_cell() [circuit: %i%s, command: %u]",
      cell.get_circuit_id() & 0x7FFFFFFF,
      (cell.get_circuit_id() & 0x80000000 ? " (MSB set)" : ""),
      cell.get_command());
  }

  switch (cell.get_command())
  {
    case cell_command::created:
      handle_created_cell(cell);
      break;

    case cell_command::destroy:
      handle_destroyed_cell(cell);
      break;

    case cell_command::relay:
    {
      relay_cell decrypted_relay_cell = decrypt(cell);

      if (decrypted_relay_cell.is_relay_cell_valid() == false)
      {
        mini_error("circuit::handle_cell() cannot decrypt relay cell, destroying circuit");
        destroy();
        break;
      }

      mini_debug(
        "tor_socket::recv_cell() [circuit: %i%s, stream: %u, command: %u, relay_command: %u, payload_size: %u]",
        decrypted_relay_cell.get_circuit_id() & 0x7FFFFFFF,
        (decrypted_relay_cell.get_circuit_id() & 0x80000000 ? " (MSB set)" : ""),
        decrypted_relay_cell.get_stream_id(),
        decrypted_relay_cell.get_command(),
        decrypted_relay_cell.get_relay_command(),
        decrypted_relay_cell.get_relay_payload().get_size());

      switch (decrypted_relay_cell.get_relay_command())
      {
        case cell_command::relay_truncated:
          handle_relay_truncated_cell(decrypted_relay_cell);
          break;

        case cell_command::relay_end:
          handle_relay_end_cell(decrypted_relay_cell);
          break;

        case cell_command::relay_connected:
          handle_relay_connected_cell(decrypted_relay_cell);
          break;

        case cell_command::relay_extended:
          handle_relay_extended_cell(decrypted_relay_cell);
          break;

        case cell_command::relay_data:
          handle_relay_data_cell(decrypted_relay_cell);
          break;

        case cell_command::relay_sendme:
          handle_relay_sendme_cell(decrypted_relay_cell);
          break;

        case cell_command::relay_command_rendezvous2:
          handle_relay_extended_cell(decrypted_relay_cell);
          set_state(state::rendezvous_completed);
          break;

        case cell_command::relay_command_rendezvous_established:
          set_state(state::rendezvous_established);
          break;

        case cell_command::relay_command_introduce_ack:
          set_state(state::rendezvous_introduced);
          break;

        default:
          mini_warning(
            "tor_socket::recv_cell() !! unhandled relay cell [ relay_command: %u ]",
            decrypted_relay_cell.get_relay_command());
          break;
      }
    }
    break;

    default:
      mini_warning(
        "tor_socket::recv_cell() !! unhandled cell [ command: %u ]",
        cell.get_command());
      break;
  }
}

void
circuit::handle_created_cell(
  cell& cell
  )
{
  //
  // finish the handshake.
  //
  _extend_node->set_shared_secret(
    cell.get_payload().slice(0, DH_LEN),
    cell.get_payload().slice(DH_LEN, DH_LEN + HASH_LEN));

  if (_extend_node->has_valid_crypto_state())
  {
    _node_list.add(_extend_node);

    //
    // we're ready here.
    //
    _extend_node = nullptr;
    set_state(state::ready);
  }
  else
  {
    mini_error("circuit::handle_created_cell() extend node [ %s ] has invalid crypto state", _extend_node->get_onion_router()->get_name());

    destroy();
  }
}

void
circuit::handle_destroyed_cell(
  cell& cell
  )
{
  MINI_UNREFERENCED(cell);

  destroy();
}

void
circuit::handle_relay_extended_cell(
  relay_cell& cell
  )
{
  //
  // finish the handshake.
  //
  _extend_node->set_shared_secret(
    cell.get_relay_payload().slice(0, DH_LEN),
    cell.get_relay_payload().slice(DH_LEN, DH_LEN + HASH_LEN));

  if (_extend_node->has_valid_crypto_state())
  {
    _node_list.add(_extend_node);

    //
    // we're ready here.
    //
    _extend_node = nullptr;
    set_state(state::ready);
  }
  else
  {
    mini_error("circuit::handle_created_cell() extend node [ %s ] has invalid crypto state", _extend_node->get_onion_router()->get_name());

    destroy();
  }
}

void
circuit::handle_relay_data_cell(
  relay_cell& cell
  )
{
  //
  // decrement deliver window on circuit node.
  //
  cell.get_circuit_node()->decrement_deliver_window();
  if (cell.get_circuit_node()->consider_sending_sendme())
  {
    send_relay_sendme_cell(nullptr);
  }

  if (tor_stream* stream = get_stream_by_id(cell.get_stream_id()))
  {
    stream->append_to_recv_buffer(cell.get_relay_payload());

    //
    // decrement window on stream.
    //
    stream->decrement_deliver_window();
    if (stream->consider_sending_sendme())
    {
      send_relay_sendme_cell(stream);
    }
  }
}

void
circuit::handle_relay_sendme_cell(
  relay_cell& cell
  )
{
  if (cell.get_stream_id() == 0)
  {
    cell.get_circuit_node()->increment_package_window();
  }
  else
  {
    if (tor_stream* stream = get_stream_by_id(cell.get_stream_id()))
    {
      stream->increment_package_window();
    }
  }
}

void
circuit::handle_relay_connected_cell(
  relay_cell& cell
  )
{
  if (tor_stream* stream = get_stream_by_id(cell.get_stream_id()))
  {
    stream->set_state(tor_stream::state::ready);
  }

  set_state(state::ready);
}

void
circuit::handle_relay_truncated_cell(
  relay_cell& cell
  )
{
  //
  // tor-spec.txt
  // 5.4.
  //
  // To tear down part of a circuit, the OP may send a RELAY_TRUNCATE cell
  // signaling a given OR (Stream ID zero).  That OR sends a DESTROY
  // cell to the next node in the circuit, and replies to the OP with a
  // RELAY_TRUNCATED cell.
  //
  // [Note: If an OR receives a TRUNCATE cell and it has any RELAY cells
  // still queued on the circuit for the next node it will drop them
  // without sending them.  This is not considered conformant behavior,
  // but it probably won't get fixed until a later version of Tor.  Thus,
  // clients SHOULD NOT send a TRUNCATE cell to a node running any current
  // version of Tor if a) they have sent relay cells through that node,
  // and b) they aren't sure whether those cells have been sent on yet.]
  //
  // When an unrecoverable error occurs along one connection in a
  // circuit, the nodes on either side of the connection should, if they
  // are able, act as follows:  the node closer to the OP should send a
  // RELAY_TRUNCATED cell towards the OP; the node farther from the OP
  // should send a DESTROY cell down the circuit.
  //

  MINI_UNREFERENCED(cell);

  mini_error("circuit::handle_relay_truncated_cell() destroying circuit");
  destroy();
}

void
circuit::handle_relay_end_cell(
  relay_cell& cell
  )
{
  if (tor_stream* stream = get_stream_by_id(cell.get_stream_id()))
  {
    mini_debug("circuit::handle_relay_end_cell() [stream: %u, reason: %u]", cell.get_stream_id(), cell.get_relay_payload()[0]);

    stream->set_state(tor_stream::state::destroyed);
    _stream_map.remove(cell.get_stream_id());
  }
}

circuit_id_type
circuit::get_next_circuit_id(
  void
  )
{
  static volatile circuit_id_type next_circuit_id = 1;
  return next_circuit_id++;
}


tor_stream_id_type
circuit::get_next_stream_id(
  void
  )
{
  static volatile tor_stream_id_type next_stream_id = 1;
  return next_stream_id++;
}

circuit::state
circuit::get_state(
  void
  ) const
{
  return _state.get_value();
}

void
circuit::set_state(
  state new_state
  )
{
  _state.set_value(new_state);

  if (new_state == state::destroyed)
  {
    _state.cancel_all_waits();
  }
}

threading::wait_result
circuit::wait_for_state(
  state desired_state,
  timeout_type timeout
  )
{
  return _state.wait_for_value(desired_state, timeout);
}

}
