#pragma once
#include "onion_router.h"

#include <mini/time.h>
#include <mini/stack_buffer.h>
#include <mini/collections/pair_list.h>

namespace mini::tor {

class consensus
{
  public:
    struct search_criteria
    {
      collections::list<uint16_t> allowed_dir_ports;
      collections::list<uint16_t> allowed_or_ports;
      onion_router_list forbidden_onion_routers;
      onion_router::status_flags flags;
    };

  public:
    consensus(
      const string_ref cached_consensus_path = nullptr,
      bool force_download = false
      );

    ~consensus(
      void
      );

    void
    create(
      const string_ref cached_consensus_path = nullptr,
      bool force_download = false
      );

    void
    destroy(
      void
      );

    //
    // getters for onion routers
    //

    onion_router*
    get_onion_router_by_name(
      const string_ref name
      ) const;

    onion_router*
    get_onion_router_by_identity_fingerprint(
      const string_ref identity_fingerprint
      );

    onion_router_list
    get_onion_routers_by_criteria(
      const search_criteria& criteria
      ) const;

    onion_router*
    get_random_onion_router_by_criteria(
      const search_criteria& criteria
      ) const;

    string
    get_onion_router_descriptor(
      const string_ref identity_fingerprint
      );

    //
    // directories
    //

    onion_router::status_flags
    get_allowed_dir_flags(
      void
      ) const;

    void
    set_allowed_dir_flags(
      onion_router::status_flags allowed_dir_flags
      );

    const collections::list<uint16_t>&
    get_allowed_dir_ports(
      void
      ) const;

    void
    set_allowed_dir_ports(
      const collections::list<uint16_t>& allowed_dir_ports
      );

    size_t
    get_max_try_count(
      void
      ) const;

    void
    set_max_try_count(
      size_t max_try_count
      );

    string
    download_from_random_router(
      const string_ref path,
      bool only_authorities = false
      );

  private:
    friend struct consensus_parser;

    string
    download_from_random_router_impl(
      const string_ref path,
      bool only_authorities
      );

    void
    parse_consensus(
      const string_ref consensus_content,
      bool reject_invalid
      );

    onion_router::status_flags _allowed_dir_flags =
      onion_router::status_flag::fast    |
      onion_router::status_flag::valid   |
      onion_router::status_flag::running |
      onion_router::status_flag::v2dir;

    collections::list<uint16_t> _allowed_dir_ports;
    size_t _max_try_count = 3;

    collections::pair_list<string, onion_router*> _onion_router_map;
    time _valid_until;
};

}
