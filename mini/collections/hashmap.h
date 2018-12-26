#pragma once
#include <mini/collections/hashset.h>

namespace mini::collections {

namespace detail {

template <
  typename Hash
>
struct hash_hashmap
{
  template <
    typename TKey,
    typename TValue
  >
  auto
  operator()(
    const pair<TKey, TValue>& value
    ) const
  {
    return hasher(value.first);
  }

  template <
    typename TKey
  >
  auto
  operator()(
    const TKey& value
    ) const
  {
    return hasher(value);
  }

  Hash hasher;
};

template <
  typename KeyEqual
>
struct equal_to_hashmap
{
  template <
    typename TKey,
    typename TValue
  >
  constexpr bool
  operator()(
    const pair<TKey, TValue>& lhs,
    const pair<TKey, TValue>& rhs
    ) const
  {
    return equal(lhs.first, rhs.first);
  }

  template <
    typename TKey,
    typename TValue,
    typename TKeyOther
  >
  constexpr bool
  operator()(
    const pair<TKey, TValue>& lhs,
    const TKeyOther& rhs
    ) const
  {
    return equal(lhs.first, rhs);
  }

  KeyEqual equal;
};

}

template <
  typename TKey,
  typename TValue,
  typename IndexType = int,
  typename Hash = hash<TKey>,
  typename KeyEqual = equal_to<TKey>,
  typename Allocator = allocator<pair<TKey, TValue>>
>
class hashmap
  : public hashset<pair<TKey, TValue>, IndexType, detail::hash_hashmap<Hash>, detail::equal_to_hashmap<KeyEqual>, Allocator>
{
  public:
    using base_type = hashset<pair<TKey, TValue>, IndexType, detail::hash_hashmap<Hash>, detail::equal_to_hashmap<KeyEqual>, Allocator>;

    using key_type                = TKey;
    using mapped_type             = TValue;
    using value_type              = pair<TKey, TValue>;
    using size_type               = size_type;
    using difference_type         = pointer_difference_type;

    using hasher                  = Hash;
    using key_equal               = KeyEqual;
    using allocator_type          = Allocator;

    using pointer                 = value_type*;
    using const_pointer           = const value_type*;
    using reference               = value_type&;
    using const_reference         = const value_type&;

    using index_type              = typename base_type::index_type;

    using iterator                = typename base_type::iterator;
    using const_iterator          = typename base_type::const_iterator;

    //
    // constructors.
    //

    hashmap(
      void
      );

    hashmap(
      const hashmap& other
      );

    hashmap(
      hashmap&& other
      );

    hashmap(
      std::initializer_list<value_type> values
      );

    hashmap(
      size_type reserve_size
      );

    //
    // destructor.
    //

    ~hashmap(
      void
      ) = default;

    //
    // assign operators.
    //

    hashmap&
    operator=(
      const hashmap& other
      );

    hashmap&
    operator=(
      hashmap&& other
      );

    //
    // swap.
    //

    void
    swap(
      hashmap& other
      );

    //
    // element access.
    //

    mapped_type&
    operator[](
      const key_type& key
      );

    const mapped_type&
    operator[](
      const key_type& key
      ) const;

    //
    // iterators.
    //

    using base_type::begin;
    using base_type::end;

    //
    // capacity.
    //

    using base_type::is_empty;
    using base_type::get_size;
    using base_type::get_bucket_count;
    using base_type::reserve;

    //
    // lookup.
    //

    bool
    contains(
      const key_type& item
      );

    iterator
    find(
      const key_type& item
      );

    const_iterator
    find(
      const key_type& item
      ) const;

    //
    // modifiers.
    //

    iterator
    insert(
      const key_type& key,
      const mapped_type& value
      );

    iterator
    insert(
      const key_type& key,
      mapped_type&& value
      );

    using base_type::insert;
    using base_type::insert_many;
    using base_type::remove;
    using base_type::clear;

  private:
    iterator
    find_or_insert(
      const key_type& key
      );
};

}

#include "hashmap.inl"
