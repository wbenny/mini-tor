#pragma once
#include "circuit_node_crypto_state.h"
#include "crypto/key_agreement.h"

#include <mini/threading/mutex.h>

namespace mini::tor {

class circuit;

class circuit_node
{
  public:
    circuit_node(
      circuit* circuit,
      onion_router* or,
      circuit_node_type node_type = circuit_node_type::normal
      );

    circuit*
    get_circuit(
      void
      );

    onion_router*
    get_onion_router(
      void
      );

    key_agreement&
    get_key_agreement(
      void
      );

    byte_buffer
    create_onion_skin(
      void
      );

    void
    set_shared_secret(
      const crypto::big_integer& peer_public,
      const byte_buffer_ref kh
      );

    bool
    has_valid_crypto_state(
      void
      ) const;

    void
    encrypt_forward_cell(
      relay_cell& cell
      );

    bool
    decrypt_backward_cell(
      cell& cell
      );

    //
    // flow control.
    //

    void
    decrement_package_window(
      void
      );

    void
    increment_package_window(
      void
      );

    void
    decrement_deliver_window(
      void
      );

    bool
    consider_sending_sendme(
      void
      );

  private:
    static constexpr size_t window_start = 1000;
    static constexpr size_t window_increment = 100;

    circuit* _circuit;
    circuit_node_type _type;

    onion_router* _onion_router;
    ptr<circuit_node_crypto_state> _crypto_state;
    key_agreement _dh;

    size_t _package_window = window_start;
    size_t _deliver_window = window_start;
    threading::mutex _window_mutex;
};

}
