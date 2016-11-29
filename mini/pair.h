#pragma once
#include <mini/common.h>

namespace mini {

template <
  typename T,
  typename U
>
struct pair
{
  pair(
    void
    ) = default;

  pair(
    const T& first,
    const U& second
    )
    : first(first)
    , second(second)
  {

  }

  template <
    typename OTHER_T,
    typename OTHER_U
  >
  pair(
    OTHER_T&& first,
    OTHER_U&& second
    )
    : first(std::forward<OTHER_T>(first))
    , second(std::forward<OTHER_U>(second))
  {

  }

  pair(
    const pair<T, U>& other
    ) = default;

  pair(
    pair<T, U>&& other
    )
  {
    swap(other);
  }

  ~pair(
    void
    ) = default;

  pair<T, U>&
    operator=(
    const pair<T, U>& other
    ) = default;

  pair<T, U>&
  operator=(
    pair<T, U>&& other
    )
  {
    swap(other);
  }

  void
  swap(
    pair<T, U>& other
    )
  {
    mini::swap(first, other.first);
    mini::swap(second, other.second);
  }

  T first;
  U second;
};

template <
  typename T,
  typename U
>
void
swap(
  pair<T, U>& lhs,
  pair<T, U>& rhs
  )
{
  lhs.swap(rhs);
}

}
