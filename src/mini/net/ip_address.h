#pragma once
#include <mini/string.h>
#include <mini/ctl.h>

#include <winsock2.h>

namespace mini::net {

class ip_address
{
  public:
    constexpr ip_address(
      void
      ) = default;

    constexpr ip_address(
      const ip_address& other
      ) = default;

    constexpr ip_address(
      ip_address&& other
      ) = default;

    constexpr ip_address(
      uint32_t value
      )
      : _ip(value)
    {

    }

    ip_address(
      const string_ref value
      )
      : _ip(inet_addr(value.get_buffer()))
    {

    }

    ip_address&
    operator=(
      const ip_address& other
      ) = default;

    ip_address&
    operator=(
      ip_address&& other
      ) = default;

    static constexpr ip_address
    from_int(
      uint32_t value
      )
    {
      return ip_address(value);
    }

    static constexpr ip_address
    from_string(
      const char* value
      )
    {
      return ip_address(ip_string_to_int(value));
    }

    static constexpr uint32_t
    to_int(
      const char* value
      )
    {
      return ip_string_to_int(value);
    }

    string
    to_string(
      void
      ) const
    {
      return string(inet_ntoa(to_in_addr()));
    }

    in_addr
    to_in_addr(
      void
      ) const
    {
      in_addr result;
      result.S_un.S_addr = _ip;

      return result;
    }

    constexpr uint32_t
    to_int(
      void
      ) const
    {
      return _ip;
    }

  private:
    static constexpr uint32_t
    ip_string_to_int(
      const char* value
      )
    {
      return
        static_cast<uint32_t>(ctl::atoi(value)) |
        static_cast<uint32_t>(ctl::atoi(ctl::strchr(value, '.') + 1)) << 8 |
        static_cast<uint32_t>(ctl::atoi(ctl::strchr(ctl::strchr(value, '.') + 1, '.') + 1)) << 16 |
        static_cast<uint32_t>(ctl::atoi(ctl::strchr(ctl::strchr(ctl::strchr(value, '.') + 1, '.') + 1, '.') + 1)) << 24;
    }

    uint32_t _ip = 0;
};

}
