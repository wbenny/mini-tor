#pragma once
#include <mini/common.h>

namespace mini::algorithm {

template <
  typename TIterator
>
pointer_difference_type
distance(
  TIterator first,
  TIterator last
  )
{
  return last - first;
}

template <
  typename TIterator,
  typename TDistance
>
void
advance(
  TIterator& iterator,
  TDistance distance
  )
{
  iterator += distance;
}

template <
  typename TIterator,
  typename T,
  typename Compare
>
TIterator
lower_bound(
  TIterator first,
  TIterator last,
  const T& value,
  Compare comp
  )
{
  TIterator it;
  pointer_difference_type count, step;
  count = distance(first, last);

  while (count > 0)
  {
    it = first;
    step = count / 2;
    advance(it, step);
    if (comp(*it, value))
    {
      first = ++it;
      count -= step + 1;
    }
    else
    {
      count = step;
    }
  }
  return first;
}

template <
  typename TIterator,
  typename T,
  typename Compare
>
bool
binary_search(
  TIterator first,
  TIterator last,
  const T& value,
  Compare comp
  )
{
  first = lower_bound(first, last, value, comp);
  return (!(first == last) && !(value < *first));
}

}

