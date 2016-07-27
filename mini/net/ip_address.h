#pragma once
#include <mini/string.h>

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
      return ip_address(ip_to_int_chunk(value));
    }

    static constexpr uint32_t
    to_int(
      const char* value
      )
    {
      return ip_to_int_chunk(value);
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

    constexpr uint32_t
    to_int() const
    {
      return _ip;
    }

  private:
    constexpr static bool
    is_digit(
      char c
      )
    {
      return c <= '9' && c >= '0';
    }

    constexpr static uint32_t
    stoi_impl(
      const char* str,
      int value = 0
      )
    {
      return *str
        ? is_digit(*str)
          ? stoi_impl(str + 1, (*str - '0') + value * 10)
          : value
        : value;
    }

    constexpr static const char*
    strchr(
      const char* s,
      int c
      )
    {
      return *s == static_cast<char>(c) ? s
        : !*s ? nullptr
        : strchr(s + 1, c);
    }

    static constexpr uint32_t
    ip_to_int_chunk(
      const char* value
      )
    {
      return
        stoi_impl(value) |
        stoi_impl(strchr(value, '.') + 1) << 8 |
        stoi_impl(strchr(strchr(value, '.') + 1, '.') + 1) << 16 |
        stoi_impl(strchr(strchr(strchr(value, '.') + 1, '.') + 1, '.') + 1) << 24;
    }

    uint32_t _ip = 0;
};

}
