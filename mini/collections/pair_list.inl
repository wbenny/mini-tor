#include "pair_list.h"

namespace mini::collections {

template <
  typename TKEY,
  typename TVALUE
>
pair_list<TKEY, TVALUE>::pair_list(
  void
  )
{

}

template <
  typename TKEY,
  typename TVALUE
>
pair_list<TKEY, TVALUE>::pair_list(
  const pair_list& other
  )
{

}

template <
  typename TKEY,
  typename TVALUE
>
pair_list<TKEY, TVALUE>::pair_list(
  pair_list&& other
  )
{
  swap(other);
}

template <
  typename TKEY,
  typename TVALUE
>
pair_list<TKEY, TVALUE>::~pair_list(
  void
  )
{

}

template <
  typename TKEY,
  typename TVALUE
>
TVALUE&
pair_list<TKEY, TVALUE>::operator[](
  const TKEY& key
  )
{
  TVALUE* value = find(key);

  if (value)
  {
    return *value;
  }
  else
  {
    return insert(key, TVALUE()).second;
  }
}

template <
  typename TKEY,
  typename TVALUE
>
pair<TKEY, TVALUE>&
pair_list<TKEY, TVALUE>::insert(
  const TKEY& key,
  const TVALUE& value
  )
{
  _buffer.add(pair<TKEY, TVALUE>(key, value));

  return _buffer[get_size()];
}

template <
  typename TKEY,
  typename TVALUE
>
pair<TKEY, TVALUE>&
pair_list<TKEY, TVALUE>::insert(
  TKEY&& key,
  TVALUE&& value
  )
{
  _buffer.add(pair<TKEY, TVALUE>(std::forward<TKEY>(key), std::forward<TVALUE>(value)));

  return _buffer[get_size()];
}

template <
  typename TKEY,
  typename TVALUE
>
pair<TKEY, TVALUE>&
pair_list<TKEY, TVALUE>::insert(
  const pair<TKEY, TVALUE>& pair
  )
{
  _buffer.add(pair);

  return _buffer[get_size()];
}

template <
  typename TKEY,
  typename TVALUE
>
pair<TKEY, TVALUE>&
pair_list<TKEY, TVALUE>::insert(
  pair<TKEY, TVALUE>&& pair
  )
{
  _buffer.add(std::move(pair));

  return _buffer[get_size()];
}

template <
  typename TKEY,
  typename TVALUE
>
TVALUE*
pair_list<TKEY, TVALUE>::find(
  const TKEY& key
  )
{
  auto&& result = find_pair(key);

  if (result)
  {
    return &result->second;
  }

  return nullptr;
}

template <
  typename TKEY,
  typename TVALUE
>
pair<TKEY, TVALUE>*
pair_list<TKEY, TVALUE>::find_pair(
  const TKEY& key
  )
{
  for (auto&& e : _buffer)
  {
    if (e.first == key)
    {
      return &e;
    }
  }

  return nullptr;
}

template <
  typename TKEY,
  typename TVALUE
>
const TVALUE*
pair_list<TKEY, TVALUE>::find(
  const TKEY& key
  ) const
{
  for (auto&& e : _buffer)
  {
    if (e.first == key)
    {
      return &e;
    }
  }

  return nullptr;
}

template <
  typename TKEY,
  typename TVALUE
>
void
pair_list<TKEY, TVALUE>::remove(
  const TKEY& key
  )
{
  auto&& result = find_pair(key);

  if (result)
  {
    _buffer.remove_at(result - _buffer.get_buffer());
  }
}

template <
  typename TKEY,
  typename TVALUE
>
void
pair_list<TKEY, TVALUE>::reserve(
  size_type new_capacity
  )
{
  return _buffer.reserve(new_capacity);
}

template <
  typename TKEY,
  typename TVALUE
>
void
pair_list<TKEY, TVALUE>::clear(
  void
  )
{
  return _buffer.clear();
}

template <
  typename TKEY,
  typename TVALUE
>
TVALUE&
pair_list<TKEY, TVALUE>::first_value(
  void
  )
{
  return _buffer[0].second;
}

template <
  typename TKEY,
  typename TVALUE
>
const TVALUE&
pair_list<TKEY, TVALUE>::first_value(
  void
  ) const
{
  return _buffer[0].second;
}

template <
  typename TKEY,
  typename TVALUE
>
TVALUE&
pair_list<TKEY, TVALUE>::last_value(
  void
  )
{
  return _buffer[get_size() - 1].second;
}

template <
  typename TKEY,
  typename TVALUE
>
const TVALUE&
pair_list<TKEY, TVALUE>::last_value(
  void
  ) const
{
  return _buffer[get_size() - 1].second;
}

template <
  typename TKEY,
  typename TVALUE
>
pair<TKEY, TVALUE>*
pair_list<TKEY, TVALUE>::begin(
  void
  )
{
  return _buffer.begin();
}

template <
  typename TKEY,
  typename TVALUE
>
const pair<TKEY, TVALUE>*
pair_list<TKEY, TVALUE>::begin(
  void
  ) const
{
  return _buffer.begin();
}

template <
  typename TKEY,
  typename TVALUE
>
pair<TKEY, TVALUE>*
pair_list<TKEY, TVALUE>::end(
  void
  )
{
  return _buffer.end();
}

template <
  typename TKEY,
  typename TVALUE
>
const pair<TKEY, TVALUE>*
pair_list<TKEY, TVALUE>::end(
  void
  ) const
{
  return _buffer.end();
}

template <
  typename TKEY,
  typename TVALUE
>
bool
pair_list<TKEY, TVALUE>::is_empty(
  void
  ) const
{
  return _buffer.is_empty();
}

template <
  typename TKEY,
  typename TVALUE
>
bool
pair_list<TKEY, TVALUE>::contains(
  const TKEY& key
  ) const
{
  return find(key) != nullptr;
}

template <
  typename TKEY,
  typename TVALUE
>
void
pair_list<TKEY, TVALUE>::swap(
  pair_list<TKEY, TVALUE>& other
  )
{
  _buffer.swap(other._buffer);
}

template <
  typename TKEY,
  typename TVALUE
>
size_type
pair_list<TKEY, TVALUE>::get_size(
  void
  ) const
{
  return _buffer.get_size() ? _buffer.get_size() - 1 : 0;
}

template <
  typename TKEY,
  typename TVALUE
>
size_type
pair_list<TKEY, TVALUE>::get_capacity(
  void
  ) const
{
  return _buffer.get_capacity();
}

}
