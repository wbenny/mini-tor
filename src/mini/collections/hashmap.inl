#include "hashmap.h"

namespace mini::collections {


template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::hashmap(
  void
  ) : base_type()
{

}

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::hashmap(
  const hashmap& other
  ) : base_type(other)
{

}

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::hashmap(
  hashmap&& other
  ) : base_type(std::move(other))
{

}

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::hashmap(
  std::initializer_list<value_type> values
  ) : base_type(values)
{

}

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::hashmap(
  size_type reserve_size
  )
  : base_type(reserve_size)
{

}

//
// assign operators.
//

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>&
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::operator=(
  const hashmap& other
  )
{
  return base_type::operator=(other);
}

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>&
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::operator=(
  hashmap&& other
  )
{
  return base_type::operator=(std::move(other));
}

//
// swap.
//

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
void
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::swap(
  hashmap& other
  )
{
  base_type::swap(other);
}

//
// element access.
//

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
typename hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::mapped_type&
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::operator[](
  const key_type& key
  )
{
  return find_or_insert(key)->second;
}

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
const typename hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::mapped_type&
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::operator[](
  const key_type& key
  ) const
{
  return find_or_insert(key)->second;
}

//
// lookup.
//

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
bool
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::contains(
  const key_type& item
  )
{
  return base_type::find_generic(item) != end();
}

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
typename hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::iterator
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::find(
  const key_type& item
  )
{
  return base_type::find_generic(item);
}

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
typename hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::const_iterator
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::find(
  const key_type& item
  ) const
{
  return base_type::find_generic(item);
}

//
// modifiers.
//

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
typename hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::iterator
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::insert(
  const key_type& key,
  const mapped_type& value
  )
{
  return base_type::insert(pair(key, value));
}

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
typename hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::iterator
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::insert(
  const key_type& key,
  mapped_type&& value
  )
{
  return base_type::insert(pair(key, std::move(value)));
}

//
// private methods.
//

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash_hashmap<pair<TKey, TValue>>,
  typename KeyEqual = equal_to_hashmap<pair<TKey, TValue>>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
typename hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::iterator
hashmap<TKey, TValue, IndexType, Hash, KeyEqual, Allocator>::find_or_insert(
  const key_type& key
  )
{
  auto it = base_type::find_generic(key);

  return it == base_type::end()
    ? insert(key, mapped_type{})
    : it;
}

}
