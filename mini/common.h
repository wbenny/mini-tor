#pragma once
#include <mini/crt/crt0.h>

#include <cstdint>
#include <cstddef>
#include <cstdarg>
#include <utility>

#ifdef min
# undef min
#endif

#ifdef max
# undef max
#endif

#define MINI_UNREFERENCED(p)        \
  (void)(p)

//
// ref: http://stackoverflow.com/a/19532607
//
#define MINI_UNREFERENCED_PARAMETER_PACK(p) \
  MINI_UNREFERENCED_PARAMETER_PACK_impl{ p... }

struct MINI_UNREFERENCED_PARAMETER_PACK_impl
{
  template <
    typename... ARGS
  >
  MINI_UNREFERENCED_PARAMETER_PACK_impl(
    const ARGS&...
    )
  {

  }
};

//
// this macro marks unreachable code
//
#if defined(__clang__)
# define MINI_UNREACHABLE   __builtin_unreachable()
#elif defined(_MSC_VER)
# define MINI_UNREACHABLE   __assume(0)
#else
# define MINI_UNREACHABLE
#endif

#if defined(_DEBUG)
# define MINI_CONFIG_DEBUG
#endif

#if defined(_M_IX86)
# define MINI_ARCH_X86
#elif defined(_M_X64)
# define MINI_ARCH_X64
#else
# error "Unknown architecture!"
#endif

#define MINI_MAKE_NONCONSTRUCTIBLE(type)  \
  type(                                   \
    void                                  \
    ) = delete;                           \

#define MINI_MAKE_NONCOPYABLE(type)       \
  type(                                   \
    const type&                           \
    ) = delete;                           \
                                          \
  type&                                   \
  operator=(                              \
    const type&                           \
    ) = delete;

#define MINI_MAKE_NONMOVABLE(type)        \
  type(                                   \
    type&&                                \
    ) = delete;                           \
                                          \
  type&                                   \
  operator=(                              \
    type&&                                \
    ) = delete;

#ifdef MINI_CONFIG_DEBUG
# define mini_assert(expression) ::mini::assert(!!(expression), #expression, __FILE__, __LINE__);
#else
# define mini_assert(expression)
#endif

#define mini_break_if(expression) if (expression) { break; } else;

#define mini_sizeof_struct_member(struct, member) sizeof(((struct*)nullptr)->member)

namespace mini {

using byte_type               = uint8_t;
using size_type               = size_t;
using file_size_type          = uint64_t;
using pointer_difference_type = ptrdiff_t;

struct no_init_tag {};

struct little_endian_tag {};
struct big_endian_tag    {};

enum class endianness
{
  little_endian,
  big_endian,
};

static constexpr endianness current_endianness = endianness::little_endian;

static constexpr size_type size_type_max = static_cast<size_type>(-1);

template <
  typename T,
  size_type N
>
constexpr size_type
countof(
  const T (&)[N]
  )
{
  return N;
}

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
  T temp(std::move(lhs));
  lhs = std::move(rhs);
  rhs = std::move(temp);
}

template <
  typename T,
  size_type N
>
void
swap(
  T (&lhs)[N],
  T (&rhs)[N]
  )
{
  T temp[N];
  memcpy(reinterpret_cast<void*>(temp), reinterpret_cast<const void*>(lhs),  N);
  memcpy(reinterpret_cast<void*>(lhs),  reinterpret_cast<const void*>(rhs),  N);
  memcpy(reinterpret_cast<void*>(rhs),  reinterpret_cast<const void*>(temp), N);
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

  for (size_type i = 0; i < sizeof(T); i++)
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
