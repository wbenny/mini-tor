#pragma once
#include "onion_router.h"
#include "cell.h"

#include <mini/net/ssl_socket.h>
#include <mini/string.h>

#include <cstdint>

namespace mini { namespace tor {

class socket
{
  public:
    static const uint16_t protocol_version_max = 4;

    socket() {}

    socket(
      onion_router* first_hop)
      : _first_hop(first_hop)
    {
      _protocol_version = 3;
      connect(first_hop->ip, first_hop->or_port);
    }

    uint32_t
    get_protocol_version() const { return _protocol_version; }

  public:
    enum class state
    {
      initialising,
      ready
    };

    void
    connect(
      net::ip& ip,
      uint16_t port);

    void
    send_cell(
      const cell& c);

    cell
    recv_cell();

    void
    send_net_info();

    void
    recv_certificates(
      const cell& c);

    void
    receive_handler_loop();

    void
    set_state(
      state new_state);

    string fetch_hs_descriptor(const string& onion);

    net::ssl_socket _socket;
    onion_router* _first_hop;
    uint32_t _protocol_version;
    state _state;
};

} }
