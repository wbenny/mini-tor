#pragma once
#include <mini/algorithm.h>
#include <mini/byte_buffer.h>
#include <mini/string_ref.h>
#include <mini/stack_buffer.h>
#include <mini/time.h>
#include <mini/collections/list.h>
#include <mini/crypto/provider.h>
#include <mini/crypto/base16.h>
#include <mini/crypto/base32.h>
#include <mini/crypto/sha1.h>
#include <mini/io/memory_stream.h>
#include <mini/io/stream_wrapper.h>
#include <mini/tor/circuit.h>
#include <mini/tor/consensus.h>
#include <mini/tor/onion_router.h>
#include <mini/tor/tor_socket.h>

#include <mini/tor/parsers/introduction_point_parser.h>

namespace mini::tor {

using replica_type = uint8_t;

class hidden_service
{
  public:
    hidden_service(
      circuit* circuit,
      const string_ref onion
      );

    bool
    connect(
      void
      );

  private:
    byte_buffer
    get_secret_id(
      replica_type replica
      );

    byte_buffer
    get_descriptor_id(
      replica_type replica
      );

    void
    find_responsible_directories(
      void
      );

    onion_router_list::size_type
    fetch_hidden_service_descriptor(
      onion_router_list::size_type responsible_directory_index = 0
      );

    void
    introduce(
      void
      );

    circuit* _rendezvous_circuit;
    tor_socket& _socket;
    consensus& _consensus;

    string _onion;
    byte_buffer _permanent_id; // crypto::base32::decode(_onion)

    onion_router_list _responsible_directory_list;
    onion_router_list _introduction_point_list;

    stack_byte_buffer<20> _rendezvous_cookie;
};

}
