#pragma once
#include <mini/string.h>

namespace mini::net {

class uri
{
  public:
    uri(
      const string_ref url
      )
      : _url(url)
    {
      //
      // protocol
      //
      auto protocol_index = _url.index_of("://");

      _protocol = protocol_index != string::not_found
        ? _url.substring(0, protocol_index + 3)
        : "http://";

      //
      // domain.
      //
      auto domain_start_index = protocol_index != string::not_found
        ? protocol_index + 3
        : 0;

      auto first_slash_index = _url.index_of("/", domain_start_index);

      _host = first_slash_index != string::not_found
        ? _url.substring(domain_start_index, first_slash_index - domain_start_index)
        : _url.substring(domain_start_index);

      auto colon_index = _host.index_of(":");
      auto port = colon_index != string::not_found
        ? _host.substring(colon_index + 1)
          : _protocol == "http://"  ? "80"
          : _protocol == "https://" ? "443"
        : "0";

      _port = static_cast<uint16_t>(port.to_int());

      if (colon_index != string::not_found)
      {
        _host = _host.substring(0, colon_index);
      }

      auto first_dot = _host.last_index_of(".");
      auto second_dot = first_dot == string::not_found
        ? string::not_found
        : _host.substring(0, first_dot).last_index_of(".");

      _domain = _host.substring(second_dot == string::not_found ? 0 : second_dot + 1);
      _path = first_slash_index != string::not_found
        ? _url.substring(first_slash_index)
        : "/";
    }

    string_ref
    get_url(
      void
      ) const
    {
      return _url;
    }

    string_ref
    get_protocol(
      void
      ) const
    {
      return _protocol;
    }

    string_ref
    get_domain(
      void
      ) const
    {
      return _domain;
    }

    string_ref
    get_host(
      void
      ) const
    {
      return _host;
    }

    string_ref
    get_path(
      void
      ) const
    {
      return _path;
    }

    uint16_t
    get_port(
      void
      ) const
    {
      return _port;
    }

  private:
    string   _url;

    string   _protocol;
    string   _domain;
    string   _host;
    string   _path;
    uint16_t _port;
};

}
