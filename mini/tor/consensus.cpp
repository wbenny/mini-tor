#include "consensus.h"
#include "parsers/consensus_parser.h"

#include <mini/logger.h>
#include <mini/io/file.h>
#include <mini/net/http.h>
#include <mini/crypto/random.h>

namespace mini::tor {

//
// static constexpr char* authorities[] = {
//   "moria1 orport=9101 v3ident=D586D18309DED4CD6D57C18FDB97EFA96D330566 128.31.0.39:9131 9695 DFC3 5FFE B861 329B 9F1A B04C 4639 7020 CE31",
//   "tor26 orport=443 v3ident=14C131DFC5C6F93646BE72FA1401C02A8DF2E8B4 ipv6=[2001:858:2:2:aabb:0:563b:1526]:443 86.59.21.38:80 847B 1F85 0344 D787 6491 A548 92F9 0493 4E4E B85D",
//   "dizum orport=443 v3ident=E8A9C45EDE6D711294FADF8E7951F4DE6CA56B58 194.109.206.212:80 7EA6 EAD6 FD83 083C 538F 4403 8BBF A077 587D D755",
//   "Tonga orport=443 bridge 82.94.251.203:80 4A0C CD2D DC79 9508 3D73 F5D6 6710 0C8A 5831 F16D",
//   "gabelmoo orport=443 v3ident=ED03BB616EB2F60BEC80151114BB25CEF515B226 ipv6=[2001:638:a000:4140::ffff:189]:443 131.188.40.189:80 F204 4413 DAC2 E02E 3D6B CF47 35A1 9BCA 1DE9 7281",
//   "dannenberg orport=443 v3ident=0232AF901C31A04EE9848595AF9BB7620D4C5B2E 193.23.244.244:80 7BE6 83E6 5D48 1413 21C5 ED92 F075 C553 64AC 7123",
//   "maatuska orport=80 v3ident=49015F787433103580E3B66A1707A00E60F2D15B ipv6=[2001:67c:289c::9]:80 171.25.193.9:443 BD6A 8292 55CB 08E6 6FBE 7D37 4836 3586 E46B 3810",
//   "Faravahar orport=443 v3ident=EFCBE720AB3A82B99F9E953CD5BF50F7EEFC7B97 154.35.175.225:80 CF6D 0AAF B385 BE71 B8E1 11FC 5CFF 4B47 9237 33BC",
//   "longclaw orport=443 v3ident=23D15D965BC35114467363C165C4F724B64B4F66 ipv6=[2620:13:4000:8000:60:f3ff:fea1:7cff]:443 199.254.238.52:80 74A9 1064 6BCE EFBC D2E8 74FC 1DC9 9743 0F96 8145",
// };
//

struct authority_onion_router
{
  constexpr authority_onion_router(
    const char* name, // ignored
    const char* ip,
    uint16_t or_port, // ignored
    uint16_t dir_port
    )
    : ip(net::ip_address::from_string(ip))
    , dir_port(dir_port)
  {

  }

  const net::ip_address ip;
  const uint16_t        dir_port;
};

//
// list of directory authority routers.
//
static constexpr stack_buffer<authority_onion_router, 9> default_authority_list = {
  authority_onion_router( "moria1"    , "128.31.0.39"     , 9101 , 9131 /* , { 0x96, 0x95, 0xDF, 0xC3, 0x5F, 0xFE, 0xB8, 0x61, 0x32, 0x9B, 0x9F, 0x1A, 0xB0, 0x4C, 0x46, 0x39, 0x70, 0x20, 0xCE, 0x31 } */ ),
  authority_onion_router( "tor26"     , "86.59.21.38"     ,  443 ,   80 /* , { 0x84, 0x7B, 0x1F, 0x85, 0x03, 0x44, 0xD7, 0x87, 0x64, 0x91, 0xA5, 0x48, 0x92, 0xF9, 0x04, 0x93, 0x4E, 0x4E, 0xB8, 0x5D } */ ),
  authority_onion_router( "dizum"     , "194.109.206.212" ,  443 ,   80 /* , { 0x7E, 0xA6, 0xEA, 0xD6, 0xFD, 0x83, 0x08, 0x3C, 0x53, 0x8F, 0x44, 0x03, 0x8B, 0xBF, 0xA0, 0x77, 0x58, 0x7D, 0xD7, 0x55 } */ ),
  authority_onion_router( "Tonga"     , "82.94.251.203"   ,  443 ,   80 /* , { 0x4A, 0x0C, 0xCD, 0x2D, 0xDC, 0x79, 0x95, 0x08, 0x3D, 0x73, 0xF5, 0xD6, 0x67, 0x10, 0x0C, 0x8A, 0x58, 0x31, 0xF1, 0x6D } */ ),
  authority_onion_router( "gabelmoo"  , "131.188.40.189"  ,  443 ,   80 /* , { 0xF2, 0x04, 0x44, 0x13, 0xDA, 0xC2, 0xE0, 0x2E, 0x3D, 0x6B, 0xCF, 0x47, 0x35, 0xA1, 0x9B, 0xCA, 0x1D, 0xE9, 0x72, 0x81 } */ ),
  authority_onion_router( "dannenberg", "193.23.244.244"  ,  443 ,   80 /* , { 0x7B, 0xE6, 0x83, 0xE6, 0x5D, 0x48, 0x14, 0x13, 0x21, 0xC5, 0xED, 0x92, 0xF0, 0x75, 0xC5, 0x53, 0x64, 0xAC, 0x71, 0x23 } */ ),
  authority_onion_router( "maatuska"  , "171.25.193.9"    ,   80 ,  443 /* , { 0xBD, 0x6A, 0x82, 0x92, 0x55, 0xCB, 0x08, 0xE6, 0x6F, 0xBE, 0x7D, 0x37, 0x48, 0x36, 0x35, 0x86, 0xE4, 0x6B, 0x38, 0x10 } */ ),
  authority_onion_router( "Faravahar" , "154.35.175.225"  ,  443 ,   80 /* , { 0xCF, 0x6D, 0x0A, 0xAF, 0xB3, 0x85, 0xBE, 0x71, 0xB8, 0xE1, 0x11, 0xFC, 0x5C, 0xFF, 0x4B, 0x47, 0x92, 0x37, 0x33, 0xBC } */ ),
  authority_onion_router( "longclaw"  , "199.254.238.52"  ,  443 ,   80 /* , { 0x74, 0xA9, 0x10, 0x64, 0x6B, 0xCE, 0xEF, 0xBC, 0xD2, 0xE8, 0x74, 0xFC, 0x1D, 0xC9, 0x97, 0x43, 0x0F, 0x96, 0x81, 0x45 } */ ),
};


consensus::consensus(
  const string_ref cached_consensus_path,
  bool force_download
  )
{
  create(cached_consensus_path, force_download);
}

consensus::~consensus(
  void
  )
{
  destroy();
}

void
consensus::create(
  const string_ref cached_consensus_path,
  bool force_download
  )
{
  string consensus_content;
  bool have_valid_consensus = false;

  //
  // if no path to the cached consensus file
  // was provided, we have to download it.
  //
  if (cached_consensus_path.is_empty() || !io::file::exists(cached_consensus_path))
  {
    force_download = true;
  }

  while (!have_valid_consensus)
  {
    consensus_content = force_download
      ? download_from_random_authority("/tor/status-vote/current/consensus")
      : io::file::read_to_string(cached_consensus_path);

    parse_consensus(consensus_content);

    have_valid_consensus = _valid_until >= time::now();

    //
    // if the consensus is invalid, we have to download it anyway.
    //
    if (!have_valid_consensus)
    {
      force_download = true;
    }
  }

  //
  // save the consensus content, if the path was provided.
  //
  if (force_download && !cached_consensus_path.is_empty())
  {
    io::file::write_from_string(cached_consensus_path, consensus_content);
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
consensus::get_onion_router_descriptor(
  const string_ref identity_fingerprint
  )
{
  return download_from_random_authority("/tor/server/fp/" + identity_fingerprint);
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
      if (criteria.allowed_dir_ports.index_of(router->get_dir_port()) == collections::list<uint16_t>::not_found)
      {
        continue;
      }
    }

    if (!criteria.allowed_or_ports.is_empty())
    {
      if (criteria.allowed_or_ports.index_of(router->get_or_port()) == collections::list<uint16_t>::not_found)
      {
        continue;
      }
    }

    if (!criteria.forbidden_onion_routers.is_empty())
    {
      if (criteria.forbidden_onion_routers.index_of(router) != onion_router_list::not_found)
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

onion_router*
consensus::get_random_onion_router_by_criteria(
  const search_criteria& criteria
  ) const
{
  auto routers = get_onion_routers_by_criteria(criteria);

  return !routers.is_empty()
    ? routers[0]
    : nullptr;
}

string
consensus::download_from_random_authority(
  const string_ref path
  )
{
  net::ip_address ip;
  uint16_t port;

  auto authority = default_authority_list[
    mini::crypto::random_device->get_random(default_authority_list.get_size())
  ];

  ip = authority.ip;
  port = authority.dir_port;

  mini_debug(
    "consensus::download_from_random_authority() [path: http://%s:%u%s]",
    ip.to_string().get_buffer(),
    port,
    path.get_buffer());

  return net::http::client::get(ip.to_string(), port, path);
}

void
consensus::parse_consensus(
  const string_ref consensus_content
  )
{
  //
  // clear the map first.
  //
  _onion_router_map.clear();

  //
  // parse the consensus document.
  //
  consensus_parser parser;
  parser.parse(*this, consensus_content);
}

}
