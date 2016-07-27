#pragma once
#include "cell.h"

namespace mini::tor {

class circuit_node;
class tor_stream;

class relay_cell
  : public cell
{
  public:
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

    tor_stream_id_type
    get_stream_id(
      void
      ) const;

    tor_stream*
    get_stream(
      void
      );

    cell_command
    get_relay_command(
      void
      ) const;

    circuit_node*
    get_circuit_node(
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

    bool
    is_relay_cell_valid(
      void
      ) const;

  private:
    circuit_node* _circuit_node = nullptr;
    cell_command _relay_command = (cell_command)0;
    tor_stream_id_type _stream_id = 0;
    uint8_t _digest[4];
    byte_buffer _relay_payload;
};

}
