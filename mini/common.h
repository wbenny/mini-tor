#pragma once
#include <mini/crt/crt0.h>

#include <cstdint>
#include <cstddef>
#include <cstdarg>

#ifdef min
# undef min
#endif

#ifdef max
# undef max
#endif

#define MINI_UNREACHABLE            \
  __assume(0)

#define MINI_MAKE_NONCOPYABLE(type) \
  type(                             \
    const type&                     \
    ) = delete;                     \
                                    \
  type&                             \
  operator=(                        \
    const type&                     \
    ) = delete;

#ifdef _DEBUG
#define mini_assert(expression) ::mini::assert(!!(expression), #expression, __FILE__, __LINE__);
#else
#define mini_assert(expression)
#endif

#define mini_break_if(expression) if (expression) { break; } else;

namespace mini {

using byte_type = uint8_t;
using size_type = size_t;
using pointer_difference_type = ptrdiff_t;

static constexpr size_type size_type_max = static_cast<size_type>(-1);

template <
  typename T
>
T
min(
  T a,
  T b
  )
{
  return (a < b) ? a : b;
}

template <
  typename T
>
T
max(
  T a,
  T b
  )
{
  return (a > b) ? a : b;
}

template <
  typename T
>
T
clamp(
  T value,
  T low,
  T high
  )
{
  return min(low, max(value, high));
}

template <
  typename T
>
T
round_up_to_multiple(
  T value,
  T multiple
  )
{
  return ((value + multiple - 1) / multiple) * multiple;
}

template <
  typename T
>
void
swap(
  T& lhs,
  T& rhs
  )
{
  T temp(lhs);
  lhs = rhs;
  rhs = temp;
}

template <
  typename T
>
T
swap_endianness(
  T u
  )
{
  union
  {
    T u;
    unsigned char u8[sizeof(T)];
  } source, dest;

  source.u = u;

  for (size_t i = 0; i < sizeof(T); i++)
  {
    dest.u8[i] = source.u8[sizeof(T) - i - 1];
  }

  return dest.u;
}

void
assert(
  int expression,
  const char* expression_str,
  const char* filename,
  int line
  );

}
