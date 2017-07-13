#pragma once
#include "common.h"

#include <mini/byte_buffer.h>
#include <mini/io/stream.h>

namespace mini::tor {

//
// 5.4.
//
// The payload of a RELAY_TRUNCATED or DESTROY cell contains a single octet,
// describing why the circuit is being closed or truncated.
//

enum class cell_destroy_reason : uint8_t
{
  none                           =  0, // no reason given
  protocol                       =  1, // tor protocol violation
  internal                       =  2, // internal error
  requested                      =  3, // a client sent a TRUNCATE command
  hibernating                    =  4, // not currently operating; trying to save bandwidth
  resource_limit                 =  5, // out of memory, sockets, or circuit IDs
  connection_failed              =  6, // unable to reach relay
  onion_router_identity          =  7, // connected to relay, but its OR identity was not as expected
  onion_router_connection_closed =  8, // the OR connection that was carrying this circuit died
  finished                       =  9, // the circuit has expired for being dirty or old
  timeout                        = 10, // circuit construction took too long
  destroyed                      = 11, // the circuit was destroyed w/o client TRUNCATE
  no_such_service                = 12, // request for unknown hidden service
};

enum class cell_command : uint8_t
{
  //
  // cell commands.
  //
  padding = 0,
  create = 1,
  created = 2,
  relay = 3,
  destroy = 4,
  create_fast = 5,
  created_fast = 6,
  versions = 7,
  netinfo = 8,
  relay_early = 9,
  create2 = 10,
  created2 = 11,

  //
  // relay commands.
  //
  relay_begin = 1,
  relay_data = 2,
  relay_end = 3,
  relay_connected = 4,
  relay_sendme = 5,
  relay_extend = 6,
  relay_extended = 7,
  relay_truncate = 8,
  relay_truncated = 9,
  relay_drop = 10,
  relay_resolve = 11,
  relay_resolved = 12,
  relay_begin_dir = 13,
  relay_extend2 = 14,
  relay_extended2 = 15,

  //
  // rendezvous commands.
  //
  relay_command_establish_intro = 32,
  relay_command_establish_rendezvous = 33,
  relay_command_introduce1 = 34,
  relay_command_introduce2 = 35,
  relay_command_rendezvous1 = 36,
  relay_command_rendezvous2 = 37,
  relay_command_intro_established = 38,
  relay_command_rendezvous_established = 39,
  relay_command_introduce_ack = 40,

  //
  // variable-length cell commands.
  //
  vpadding = 128,
  certs = 129,
  auth_challenge = 130,
  authenticate = 131,
  authorize = 132,
};

class cell
{
  public:
    //
    // tor-spec.txt
    // 0.2.
    //    CELL_LEN(v) -- The length of a Tor cell, in bytes, for link protocol
    //       version v.
    //        CELL_LEN(v) = 512    if v is less than 4;
    //                    = 514    otherwise.
    //
    static constexpr size_type size = 514;

    //
    // PAYLOAD_LEN -- The longest allowable cell payload, in bytes. (509)
    //
    static constexpr size_type payload_size = 509;

    cell(
      void
      ) = default;

    cell(
      const cell& other
      ) = default;

    cell(
      cell&& other
      );

    //
    // create cell manually.
    //
    cell(
      circuit_id_type circuit_id,
      cell_command command,
      const byte_buffer_ref payload = byte_buffer_ref()
      );

    ~cell(
      void
      ) = default;

    cell&
    operator=(
      const cell& other
      ) = default;

    void
    swap(
      cell& other
      );

    circuit_id_type
    get_circuit_id(
      void
      ) const;

    void
    set_circuit_id(
      circuit_id_type circuit_id
      );

    cell_command
    get_command(
      void
      ) const;

    void
    set_command(
      cell_command command
      );

    byte_buffer_ref
    get_payload(
      void
      ) const;

    void
    set_payload(
      const byte_buffer_ref payload
      );

    byte_buffer
    get_bytes(
      protocol_version_type protocol_version
      ) const;

    bool
    is_recognized(
      void
      ) const;

    bool
    is_valid(
      void
      ) const;

    void
    mark_as_valid(
      void
      );

    static bool
    is_variable_length_cell_command(
      cell_command command
      );

  protected:
    circuit_id_type _circuit_id = 0;
    cell_command _command = (cell_command)0;
    byte_buffer _payload;
    bool _is_valid = false;
};

}
