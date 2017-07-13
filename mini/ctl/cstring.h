#pragma once

namespace mini::ctl {

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

constexpr static int strlen(
  const char* s
  )
{
  return *s == '\0'
    ? 0
    : strlen(s + 1) + 1;
}

}
