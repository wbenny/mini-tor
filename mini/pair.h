#pragma once
#include <mini/common.h>

namespace mini {

template <
  typename TFirst,
  typename TSecond
>
struct pair
{
  using first_type = TFirst;
  using second_type = TSecond;

  //
  // constructors.
  //

  constexpr pair(
    void
    ) = default;

  constexpr pair(
    const pair& other
    ) = default;

  constexpr pair(
    pair&& other
    ) = default;

  constexpr pair(
    const TFirst& first,
    const TSecond& second
    )
    : first(first)
    , second(second)
  {

  }

  constexpr pair(
    TFirst&& first,
    TSecond&& second
    )
    : first(std::forward<TFirst>(first))
    , second(std::forward<TSecond>(second))
  {

  }

  template <
    typename TOtherFirst,
    typename TOtherSecond
  >
  constexpr pair(
    const TOtherFirst& first,
    const TOtherSecond& second
    )
    : first(first)
    , second(second)
  {

  }

  template <
    typename TOtherFirst,
    typename TOtherSecond
  >
  constexpr pair(
    TOtherFirst&& first,
    TOtherSecond&& second
    )
    : first(std::forward<TOtherFirst>(first))
    , second(std::forward<TOtherSecond>(second))
  {

  }

  template <
    typename TOtherFirst,
    typename TOtherSecond
  >
  constexpr pair(
    const pair<TOtherFirst, TOtherSecond>& other
    )
    : first(other.first)
    , second(other.second)
  {

  }

  template <
    typename TOtherFirst,
    typename TOtherSecond
  >
  constexpr pair(
    pair<TOtherFirst, TOtherSecond>&& other
    )
    : first(std::forward<TOtherFirst>(other.first))
    , second(std::forward<TOtherSecond>(other.second))
  {

  }

  //
  // destructor.
  //

  ~pair(
    void
    ) = default;

  //
  // assign operators.
  //

  constexpr pair&
  operator=(
    const pair& other
    ) = default;

  constexpr pair&
  operator=(
    pair&& other
    ) = default;

  template <
    typename TOtherFirst,
    typename TOtherSecond
  >
  constexpr pair&
  operator=(
    const pair<TOtherFirst, TOtherSecond>& other
    )
  {
    first = other.first;
    second = other.second;
  }

  template <
    typename TOtherFirst,
    typename TOtherSecond
  >
  constexpr pair&
  operator=(
    pair<TOtherFirst, TOtherSecond>&& other
    )
  {
    first = std::forward<TOtherFirst>(other.first);
    second = std::forward<TOtherSecond>(other.second);
  }

  //
  // swap.
  //

  constexpr void
  swap(
    pair& other
    )
  {
    mini::swap(first, other.first);
    mini::swap(second, other.second);
  }

  first_type  first;
  second_type second;
};

//
// non-member operations.
//

template<
  typename TFirst,
  typename TSecond
  >
constexpr bool
operator==(
  const pair<TFirst, TSecond>& lhs,
  const pair<TFirst, TSecond>& rhs
  )
{
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<
  typename TFirst,
  typename TSecond
>
constexpr bool
operator!=(
  const pair<TFirst, TSecond>& lhs,
  const pair<TFirst, TSecond>& rhs
  )
{
  return !(lhs == rhs);
}

template<
  typename TFirst,
  typename TSecond
>
constexpr bool
operator<(
  const pair<TFirst, TSecond>& lhs,
  const pair<TFirst, TSecond>& rhs
  )
{
  return (lhs.first  < rhs.first  ||
       (!(rhs.first  < lhs.first) &&
          lhs.second < rhs.second));
}

template<
  typename TFirst,
  typename TSecond
>
constexpr bool
operator<=(
  const pair<TFirst, TSecond>& lhs,
  const pair<TFirst, TSecond>& rhs
  )
{
  return !(rhs < lhs);
}

template<
  typename TFirst,
  typename TSecond
>
constexpr bool
operator>(
  const pair<TFirst, TSecond>& lhs,
  const pair<TFirst, TSecond>& rhs
  )
{
  return rhs < lhs;
}

template<
  typename TFirst,
  typename TSecond
>
constexpr bool
operator>=(
  const pair<TFirst, TSecond>& lhs,
  const pair<TFirst, TSecond>& rhs
  )
{
  return !(lhs < rhs);
}

template <
  typename TFirst,
  typename TSecond
>
void
swap(
  pair<TFirst, TSecond>& lhs,
  pair<TFirst, TSecond>& rhs
  )
{
  lhs.swap(rhs);
}

}
