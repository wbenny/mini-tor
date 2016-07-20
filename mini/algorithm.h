#pragma once
#include <mini/common.h>

namespace mini::algorithm {

template <
  typename ITERATOR_TYPE
>
ptrdiff_t
distance(
  ITERATOR_TYPE first,
  ITERATOR_TYPE last
  )
{
  return last - first;
}

template <
  typename ITERATOR_TYPE,
  typename DISTANCE_TYPE
>
void
advance(
  ITERATOR_TYPE& iterator,
  DISTANCE_TYPE distance
  )
{
  iterator += distance;
}

template <
  typename ITERATOR_TYPE,
  typename T,
  typename COMPARE_TYPE
>
ITERATOR_TYPE
lower_bound(
  ITERATOR_TYPE first,
  ITERATOR_TYPE last,
  const T& value,
  COMPARE_TYPE comp
  )
{
  ITERATOR_TYPE it;
  ptrdiff_t count, step;
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
  typename ITERATOR_TYPE,
  typename T,
  typename COMPARE_TYPE
>
bool
binary_search(
  ITERATOR_TYPE first,
  ITERATOR_TYPE last,
  const T& value,
  COMPARE_TYPE comp
  )
{
  first = lower_bound(first, last, value, comp);
  return (!(first == last) && !(value < *first));
}

}

