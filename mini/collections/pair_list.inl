#include "pair_list.h"

namespace mini::collections {

//
// constructors.
//

template <
  typename TKey,
  typename TValue
>
pair_list<TKey, TValue>::pair_list(
  void
  )
{

}

template <
  typename TKey,
  typename TValue
>
pair_list<TKey, TValue>::pair_list(
  const pair_list& other
  )
{

}

template <
  typename TKey,
  typename TValue
>
pair_list<TKey, TValue>::pair_list(
  pair_list&& other
  )
{
  swap(other);
}

//
// destructor.
//

template <
  typename TKey,
  typename TValue
>
pair_list<TKey, TValue>::~pair_list(
  void
  )
{

}

//
// swap.
//

template <
  typename TKey,
  typename TValue
>
void
pair_list<TKey, TValue>::swap(
  pair_list<TKey, TValue>& other
  )
{
  _buffer.swap(other._buffer);
}

//
// element access.
//

template <
  typename TKey,
  typename TValue
>
TValue&
pair_list<TKey, TValue>::operator[](
  const TKey& key
  )
{
  TValue* value = find(key);

  if (value)
  {
    return *value;
  }
  else
  {
    return insert(key, TValue()).second;
  }
}

template <
  typename TKey,
  typename TValue
>
TValue&
pair_list<TKey, TValue>::first_value(
  void
  )
{
  return _buffer[0].second;
}

template <
  typename TKey,
  typename TValue
>
const TValue&
pair_list<TKey, TValue>::first_value(
  void
  ) const
{
  return _buffer[0].second;
}

template <
  typename TKey,
  typename TValue
>
TValue&
pair_list<TKey, TValue>::last_value(
  void
  )
{
  return _buffer[get_size() - 1].second;
}

template <
  typename TKey,
  typename TValue
>
const TValue&
pair_list<TKey, TValue>::last_value(
  void
  ) const
{
  return _buffer[get_size() - 1].second;
}

//
// iterators.
//

template <
  typename TKey,
  typename TValue
>
pair<TKey, TValue>*
pair_list<TKey, TValue>::begin(
  void
  )
{
  return _buffer.begin();
}

template <
  typename TKey,
  typename TValue
>
const pair<TKey, TValue>*
pair_list<TKey, TValue>::begin(
  void
  ) const
{
  return _buffer.begin();
}

template <
  typename TKey,
  typename TValue
>
pair<TKey, TValue>*
pair_list<TKey, TValue>::end(
  void
  )
{
  return _buffer.end();
}

template <
  typename TKey,
  typename TValue
>
const pair<TKey, TValue>*
pair_list<TKey, TValue>::end(
  void
  ) const
{
  return _buffer.end();
}

//
// capacity.
//

template <
  typename TKey,
  typename TValue
>
bool
pair_list<TKey, TValue>::is_empty(
  void
  ) const
{
  return _buffer.is_empty();
}

template <
  typename TKey,
  typename TValue
>
size_type
pair_list<TKey, TValue>::get_size(
  void
  ) const
{
  return _buffer.get_size() ? _buffer.get_size() - 1 : 0;
}

template <
  typename TKey,
  typename TValue
>
size_type
pair_list<TKey, TValue>::get_capacity(
  void
  ) const
{
  return _buffer.get_capacity();
}

//
// lookup.
//


template <
  typename TKey,
  typename TValue
>
bool
pair_list<TKey, TValue>::contains(
  const TKey& key
  ) const
{
  return find(key) != nullptr;
}

template <
  typename TKey,
  typename TValue
>
TValue*
pair_list<TKey, TValue>::find(
  const TKey& key
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
  typename TKey,
  typename TValue
>
const TValue*
pair_list<TKey, TValue>::find(
  const TKey& key
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
  typename TKey,
  typename TValue
>
pair<TKey, TValue>*
pair_list<TKey, TValue>::find_pair(
  const TKey& key
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

//
// modifiers.
//

template <
  typename TKey,
  typename TValue
>
pair<TKey, TValue>&
pair_list<TKey, TValue>::insert(
  const TKey& key,
  const TValue& value
  )
{
  _buffer.add(pair<TKey, TValue>(key, value));

  return _buffer[get_size()];
}

template <
  typename TKey,
  typename TValue
>
pair<TKey, TValue>&
pair_list<TKey, TValue>::insert(
  TKey&& key,
  TValue&& value
  )
{
  _buffer.add(pair<TKey, TValue>(std::forward<TKey>(key), std::forward<TValue>(value)));

  return _buffer[get_size()];
}

template <
  typename TKey,
  typename TValue
>
pair<TKey, TValue>&
pair_list<TKey, TValue>::insert(
  const pair<TKey, TValue>& pair
  )
{
  _buffer.add(pair);

  return _buffer[get_size()];
}

template <
  typename TKey,
  typename TValue
>
pair<TKey, TValue>&
pair_list<TKey, TValue>::insert(
  pair<TKey, TValue>&& pair
  )
{
  _buffer.add(std::move(pair));

  return _buffer[get_size()];
}


template <
  typename TKey,
  typename TValue
>
void
pair_list<TKey, TValue>::remove(
  const TKey& key
  )
{
  auto&& result = find_pair(key);

  if (result)
  {
    _buffer.remove_at(result - _buffer.get_buffer());
  }
}

template <
  typename TKey,
  typename TValue
>
void
pair_list<TKey, TValue>::reserve(
  size_type new_capacity
  )
{
  return _buffer.reserve(new_capacity);
}

template <
  typename TKey,
  typename TValue
>
void
pair_list<TKey, TValue>::clear(
  void
  )
{
  return _buffer.clear();
}

}
