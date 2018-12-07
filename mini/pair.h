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

  pair(
    void
    ) = default;

  pair(
    const TFirst& first,
    const TSecond& second
    )
    : first(first)
    , second(second)
  {

  }

  template <
    typename TOtherFirst,
    typename TOtherSecond
  >
  pair(
    TOtherFirst&& first,
    TOtherSecond&& second
    )
    : first(std::forward<TOtherFirst>(first))
    , second(std::forward<TOtherSecond>(second))
  {

  }

  pair(
    const pair<TFirst, TSecond>& other
    ) = default;

  pair(
    pair<TFirst, TSecond>&& other
    )
  {
    swap(other);
  }

  ~pair(
    void
    ) = default;

  pair<TFirst, TSecond>&
  operator=(
    const pair<TFirst, TSecond>& other
    ) = default;

  pair<TFirst, TSecond>&
  operator=(
    pair<TFirst, TSecond>&& other
    )
  {
    swap(other);
    return *this;
  }

  void
  swap(
    pair<TFirst, TSecond>& other
    )
  {
    mini::swap(first, other.first);
    mini::swap(second, other.second);
  }

  TFirst  first;
  TSecond second;
};

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
