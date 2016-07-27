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

    string
    get_onion_router_descriptor(
      const string_ref identity_fingerprint
      );

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

  private:
    friend struct consensus_parser;

    string
    download_from_random_authority(
      const string_ref path
      );

    void
    parse_consensus(
      const string_ref consensus_content
      );

    collections::pair_list<string, onion_router*> _onion_router_map;
    time _valid_until;
};

}
