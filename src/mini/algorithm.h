#ifdef min
# undef min
#endif

#ifdef max
# undef max
#endif

#ifndef MINI_ALGORITHM_INCLUDED
#define MINI_ALGORITHM_INCLUDED

#include <mini/common.h>
#include <mini/memory.h>

#include <type_traits>
#include <algorithm>

namespace mini::algorithm {

template <
  typename T
>
inline constexpr T
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
inline constexpr T
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
inline constexpr T
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
inline constexpr T
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
inline T
nearest_power_of_2(
  T value
  )
{
  static_assert(std::is_unsigned_v<T>);

  if (value < 1)
  {
    return 0;
  }

  --value;
  for (int i = 0; i; i <<= 1)
  {
    value |= value >> i;
  }
  ++value;

  return value;
}

template <
  typename T
>
inline bool
is_power_of_2(
  T value
  )
{
  static_assert(std::is_unsigned_v<T>);

  return !(value == 0) && !(value & (value - 1));
}

template<
  class T,
  typename TIterator
>
inline void
fill(
  TIterator first,
  TIterator last,
  const T& value
  )
{
  std::fill(first, last, value);
}

template <
  typename TIterator
>
inline pointer_difference_type
distance(
  TIterator first,
  TIterator last
  )
{
  return std::distance(first, last);
}

template <
  typename TIterator,
  typename TDistance
>
inline void
advance(
  TIterator& iterator,
  TDistance distance
  )
{
  std::advance(iterator, distance);
}

template <
  typename T,
  typename TIterator,
  typename Compare
>
inline TIterator
lower_bound(
  TIterator first,
  TIterator last,
  const T& value,
  Compare comp
  )
{
  return std::lower_bound(first, last, value, comp);
}

template <
  typename T,
  typename TIterator,
  typename Compare
>
inline bool
binary_search(
  TIterator first,
  TIterator last,
  const T& value,
  Compare comp
  )
{
  return std::binary_search(first, last, value, comp);
}

}

#endif