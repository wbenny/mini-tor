#include "consensus.h"

#include <mini/crypto/base64.h>
#include <mini/crypto/base16.h>
#include <mini/net/http.h>
#include <mini/io/file_stream.h>
#include <mini/string.h>
#include <mini/collections/list.h>
#include <mini/io/file.h>
#include <mini/logger.h>

#include "parsers/consensus_parser.h"

namespace mini::tor {

consensus::consensus(
  option_flags options
  )
{
  fetch_consensus(options);
}

consensus::~consensus(
  void
  )
{
  destroy();
}

void
consensus::fetch_consensus(
  option_flags options
  )
{
  string consensus_content;

  if (!options.test_flag(option_flag::force_download) && options.test_flag(option_flag::use_cache) && io::file::exists(cached_consensus_filename))
  {
    consensus_content = io::file::read_to_string(cached_consensus_filename);
    parse_consensus(consensus_content);
  }
  else if (options.test_flag(option_flag::force_download) || options.test_flag(option_flag::do_not_use_cache))
  {
    consensus_content = download_from_random_authority("/tor/status-vote/current/consensus");

    if (options.test_flag(option_flag::do_not_use_cache) == false)
    {
      io::file::write_from_string(cached_consensus_filename, consensus_content);
    }

    parse_consensus(consensus_content);
  }

  //
  // if the consensus is invalid, we have to download it anyway
  //
  if (_valid_until < time::now())
  {
    consensus_content = download_from_random_authority("/tor/status-vote/current/consensus");

    if (options.test_flag(option_flag::do_not_use_cache) == false)
    {
      io::file::write_from_string(cached_consensus_filename, consensus_content);
    }

    parse_consensus(consensus_content);
  }
}

void
consensus::destroy(
  void
  )
{
  for (auto&& onion_router : _onion_router_map)
  {
    delete onion_router.second;
  }
}

string
consensus::get_router_consensus(
  const string_ref identity_fingerprint
  )
{
  mini_info("consensus::get_router_consensus() [identity_fingerprint: %s]", identity_fingerprint.get_buffer());

  return download_from_random_authority("/tor/server/fp/" + identity_fingerprint);
}

onion_router*
consensus::get_random_onion_router_by_criteria(
  const search_criteria& criteria
  ) const
{
  for (auto&& pair : _onion_router_map)
  {
    auto router = pair.second;

    if (!criteria.allowed_dir_ports.is_empty())
    {
      if (criteria.allowed_dir_ports.index_of(router->get_dir_port()) == collections::list<string>::not_found)
      {
        continue;
      }
    }

    if (!criteria.allowed_or_ports.is_empty())
    {
      if (criteria.allowed_or_ports.index_of(router->get_or_port()) == collections::list<string>::not_found)
      {
        continue;
      }
    }

    if (!criteria.forbidden_onion_routers.is_empty())
    {
      if (criteria.forbidden_onion_routers.index_of(router) != collections::list<string>::not_found)
      {
        continue;
      }
    }

    if (criteria.flags != onion_router::status_flag::none)
    {
      if ((router->get_flags() & criteria.flags) != criteria.flags)
      {
        continue;
      }
    }

    return router;
  }

  return nullptr;
}

onion_router*
consensus::get_onion_router_by_name(
  const string_ref name
  ) const
{
  for (auto&& pair : _onion_router_map)
  {
    auto router = pair.second;

    if (name == router->get_name())
    {
      return router;
    }
  }

  return nullptr;
}

onion_router*
consensus::get_onion_router_by_identity_fingerprint(
  const string_ref identity_fingerprint
  )
{
  return _onion_router_map[identity_fingerprint];
}

onion_router_list
consensus::get_onion_routers_by_criteria(
  const search_criteria& criteria
  ) const
{
  onion_router_list result;

  for (auto&& pair : _onion_router_map)
  {
    auto router = pair.second;

    if (!criteria.allowed_dir_ports.is_empty())
    {
      if (criteria.allowed_dir_ports.index_of(router->get_dir_port()) == collections::list<string>::not_found)
      {
        continue;
      }
    }

    if (!criteria.allowed_or_ports.is_empty())
    {
      if (criteria.allowed_or_ports.index_of(router->get_or_port()) == collections::list<string>::not_found)
      {
        continue;
      }
    }

    if (!criteria.forbidden_onion_routers.is_empty())
    {
      if (criteria.forbidden_onion_routers.index_of(router) != collections::list<string>::not_found)
      {
        continue;
      }
    }

    if (criteria.flags != onion_router::status_flag::none)
    {
      if ((router->get_flags() & criteria.flags) != criteria.flags)
      {
        continue;
      }
    }

    result.add(router);
  }

  return result;
}

string
consensus::download_from_random_authority(
  const string_ref path
  )
{
  //
  // TODO:
  // chose really random authority.
  //
  mini_info("consensus::download_from_random_authority() [path: %s]", path.get_buffer());

  auto authority_line = tor::authorities[3];
  auto splitted = string(authority_line).split(" ");
  auto ip_port = splitted[3].split(":");

  string consensus_content = net::http::client::get(ip_port[0], (uint16_t)ip_port[1].to_int(), path);

  return consensus_content;
}

void
consensus::parse_consensus(
  const string& consensus_content
  )
{
  consensus_parser parser;
  parser.parse(*this, consensus_content);
}

}
