#pragma once
#include "types.h"

#include <mini/byte_buffer.h>
#include <mini/io/stream.h>

namespace mini::tor {

enum class cell_command : uint8_t
{
  //
  // cell command types.
  //
  padding = 0,
  create = 1,
  created = 2,
  relay = 3,
  destroy = 4,
  create_fast = 5,
  created_fast = 6,
  netinfo = 8,
  relay_early = 9,
  versions = 7,

  //
  // relay command types.
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

  //
  //
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
  //
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
    static constexpr size_t size = 512;
    static constexpr size_t header_size = 3;
    static constexpr size_t variable_header_size = 5;
    static constexpr size_t payload_size = size - header_size;

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

  protected:
    circuit_id_type _circuit_id = 0;
    cell_command _command = (cell_command)0;
    byte_buffer _payload;
    bool _is_valid = true;
};

}
