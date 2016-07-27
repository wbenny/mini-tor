#pragma once
#include <mini/flags.h>
#include <mini/byte_buffer.h>
#include <mini/net/ip_address.h>

namespace mini::tor {

class consensus;

class onion_router
{
  public:
    enum class status_flag : uint16_t
    {
      none = 0x0000,

      //
      // if the router is a directory authority
      //
      authority = 0x0001,

      //
      // if the router is believed to be useless as an exit node
      // (because its ISP censors it, because it is behind a restrictive
      // proxy, or for some similar reason)
      //
      bad_exit = 0x0002,

      //
      // if the router is more useful for building
      // general - purpose exit circuits than for relay circuits.The
      // path building algorithm uses this flag; see path - spec.txt.
      //
      exit = 0x0004,

      //
      // if the router is suitable for high - bandwidth circuits
      //
      fast = 0x0008,

      //
      // if the router is suitable for use as an entry guard
      //
      guard = 0x0010,

      //
      // if the router is considered a v2 hidden service directory
      //
      hsdir = 0x0020,

      //
      // if the router's identity-nickname mapping is canonical,
      // and this authority binds names
      //
      named = 0x0040,

      //
      // if any Ed25519 key in the router's descriptor or
      // microdesriptor does not reflect authority consensus
      //
      no_ed_consensus = 0x0080,

      //
      // if the router is suitable for long - lived circuits
      //
      stable = 0x0100,

      //
      // if the router is currently usable
      //
      running = 0x0200,

      //
      // if another router has bound the name used by this
      // router, and this authority binds names
      //
      unnamed = 0x0400,

      //
      // if the router has been 'validated'
      //
      valid = 0x0800,

      //
      // if the router implements the v2 directory protocol or
      // higher
      //
      v2dir = 0x1000,
    };

    using status_flags = flags<status_flag>;

  public:
    onion_router(
      consensus& consensus,
      const string_ref name,
      const string_ref ip,
      uint16_t or_port,
      uint16_t dir_port,
      const string_ref identity_fingerprint
      );

    consensus&
    get_consensus(
      void
      );

    string_ref
    get_name(
      void
      ) const;

    void
    set_name(
      const string_ref value
      );

    net::ip_address
    get_ip_address(
      void
      ) const;

    void
    set_ip_address(
      net::ip_address value
      );

    uint16_t
    get_or_port(
      void
      ) const;

    void
    set_or_port(
      uint16_t value
      );

    uint16_t
    get_dir_port(
      void
      ) const;

    void
    set_dir_port(
      uint16_t value
      );

    string_ref
    get_identity_fingerprint(
      void
      ) const;

    void
    set_identity_fingerprint(
      const string_ref value
      );

    status_flags
    get_flags(
      void
      ) const;

    void
    set_flags(
      status_flags
      );

    byte_buffer_ref
    get_onion_key(
      void
      );

    void
    set_onion_key(
      const byte_buffer_ref value
      );

    byte_buffer_ref
    get_signing_key(
      void
      );

    void
    set_signing_key(
      const byte_buffer_ref value
      );

    byte_buffer_ref
    get_service_key(
      void
      );

    void
    set_service_key(
      const byte_buffer_ref value
      );

  private:
    void
    fetch_descriptor(
      void
      );

    consensus& _consensus;

    string _name;
    net::ip_address _ip;
    uint16_t _or_port;
    uint16_t _dir_port;

    string _identity_fingerprint;
    status_flags _flags;

    byte_buffer _onion_key;
    byte_buffer _signing_key;
    byte_buffer _service_key; // for introduction point
};

using onion_router_list = collections::list<onion_router*>;

DECLARE_FLAGS_OPERATORS(onion_router::status_flags);

}
