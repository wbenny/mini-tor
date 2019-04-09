#pragma once
#include "cell.h"
#include <mini/stack_buffer.h>

namespace mini::tor {

class circuit_node;
class tor_stream;

//
// 6.3.
//
// The payload of a RELAY_END cell begins with a single 'reason' byte to
// describe why the stream is closing, plus optional data(depending on
// the reason.)
//

enum class relay_end_reason : uint8_t
{
  misc                   =  1, // catch-all for unlisted reasons
  resolve_failed         =  2, // couldn't look up hostname
  connection_refused     =  3, // remote host refused connection
  exit_policy            =  4, // OR refuses to connect to host or port
  destroy                =  5, // circuit is being destroyed
  done                   =  6, // anonymized TCP connection was closed
  timeout                =  7, // connection timed out, or OR timed out while connecting
  no_route               =  8, // routing error while attempting to contact destination
  hibernating            =  9, // OR is temporarily hibernating
  internal               = 10, // internal error at the OR
  resource_limit         = 11, // OR has no resources to fulfill request
  connection_reset       = 12, // connection was unexpectedly reset
  tor_protocol_violation = 13, // sent when closing connection because of Tor protocol violations
  not_directory          = 14, // client sent RELAY_BEGIN_DIR to a non - directory relay
};

class relay_cell
  : public cell
{
  public:
    static constexpr size_type payload_data_size = cell::payload_size - 11;

    relay_cell(
      void
      ) = default;

    relay_cell(
      circuit_node* node,
      const cell& cell
      );

    relay_cell(
      circuit_id_type circuit_id,
      cell_command command,
      circuit_node* node,
      cell_command relay_command,
      tor_stream_id_type stream_id,
      const byte_buffer_ref relay_payload
      );

    cell_command
    get_relay_command(
      void
      ) const;

    tor_stream_id_type
    get_stream_id(
      void
      ) const;

    tor_stream*
    get_stream(
      void
      );

    void
    set_digest(
      const byte_buffer_ref digest
      );

    byte_buffer_ref
    get_relay_payload(
      void
      ) const;

    void
    set_relay_payload(
      const byte_buffer_ref payload
      );

    circuit_node*
    get_circuit_node(
      void
      );

    bool
    is_relay_cell_valid(
      void
      ) const;

  private:
    circuit_node* _circuit_node = nullptr;

    //
    // tor-spec.txt
    // 6.1.
    //
    //   The payload of each unencrypted RELAY cell consists of:
    //     Relay command           [1 byte]
    //     'Recognized'            [2 bytes]
    //     StreamID                [2 bytes]
    //     Digest                  [4 bytes]
    //     Length                  [2 bytes]
    //     Data                    [PAYLOAD_LEN-11 bytes]
    //
    cell_command _relay_command = (cell_command)0;
    tor_stream_id_type _stream_id = 0;
    stack_byte_buffer<4> _digest;
    byte_buffer _relay_payload;
};

}
