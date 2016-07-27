#pragma once
#include "onion_router.h"
#include "cell.h"

#include <mini/net/ssl_socket.h>
#include <mini/threading/thread_function.h>
#include <mini/threading/mutex.h>
#include <mini/threading/locked_value.h>

namespace mini::tor {

class circuit;

class tor_socket
{
  public:
    static const protocol_version_type protocol_version_preferred = 4;

    tor_socket(
      onion_router* onion_router = nullptr
      );

    ~tor_socket(
      void
      );

    void
    connect(
      onion_router* or
      );

    void
    close(
      void
      );

    circuit*
    create_circuit(
      void
      );

    void
    remove_circuit(
      circuit* circuit
      );

    void
    send_cell(
      const cell& cell
      );

    cell
    recv_cell(
      void
      );

    protocol_version_type
    get_protocol_version(
      void
      ) const;

    onion_router*
    get_onion_router(
      void
      );

    circuit*
    get_circuit_by_id(
      circuit_id_type circuit_id
      );

    bool
    is_connected(
      void
      ) const;

  private:
    friend class circuit;

    enum state
    {
      connecting,
      handshake_in_progress,
      ready,
      closing,
      closed,
    };

    state
    get_state(
      void
      );

    void
    set_state(
      state new_state
      );

    void
    wait_for_state(
      state desired_state
      );

    void
    send_versions(
      void
      );

    void
    recv_versions(
      void
      );

    void
    send_net_info(
      void
      );

    void
    recv_net_info(
      void
      );

    void
    send_certificates(
      void
      );

    void
    recv_certificates(
      void
      );

    void
    recv_cell_loop(
      void
      );

    net::ssl_socket _socket;
    onion_router* _onion_router = nullptr;
    uint32_t _protocol_version = 3;

    threading::thread_function _recv_cell_loop_thread;
    collections::pair_list<circuit_id_type, circuit*> _circuit_map;
    threading::locked_value<state> _state = state::connecting;
};

}
