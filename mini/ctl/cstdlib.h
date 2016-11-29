#pragma once
#include "cctype.h"

namespace mini::ctl {

constexpr static int
atoi_impl(
  const char* str,
  int value
  )
{
  return *str
    ? isdigit(*str)
      ? atoi_impl(str + 1, (*str - '0') + value * 10)
      : value
    : value;
}

constexpr static int
atoi(
  const char* str
  )
{
  return atoi_impl(str, 0);
}

}
