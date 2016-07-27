#pragma once
#include <mini/string.h>

#include <winsock2.h>

namespace mini::net {

class ip_address
{
  public:
    ip_address()
      : _ip(0)
    {

    }

    ip_address(
      const ip_address& other
      ) = default;

    ip_address(
      ip_address&& other
      ) = default;

    ip_address(
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

    void
    from_int(
      uint32_t value
      )
    {
      _ip = value;
    }

    void
    from_string(
      const char* value
      )
    {
      _ip = inet_addr(value);
    }

    string
    to_string() const
    {
      return string(inet_ntoa(to_in_addr()));
    }

    in_addr
    to_in_addr() const
    {
      in_addr result;
      result.S_un.S_addr = _ip;

      return result;
    }

    uint32_t
    to_int() const
    {
      return _ip;
    }

  private:
    uint32_t _ip;
};

}
