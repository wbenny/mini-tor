#pragma once
#include <mini/pair.h>
#include <mini/collections/list.h>

namespace mini::collections {

template <
  typename TKey,
  typename TValue
>
class pair_list
{
  public:
    using value_type              = pair<TKey, TValue>;
    using size_type               = size_type;
    using difference_type         = pointer_difference_type;

    // using allocator_type          = Allocator;

    using pointer                 = value_type*;
    using const_pointer           = const value_type*;
    using reference               = value_type&;
    using const_reference         = const value_type&;

    using iterator                = pointer;
    using const_iterator          = const_pointer;

    //
    // constructors.
    //

    pair_list(
      void
      );

    pair_list(
      const pair_list& other
      );

    pair_list(
      pair_list&& other
      );

    //
    // destructor.
    //

    ~pair_list(
      void
      );

    //
    // swap.
    //

    void
    swap(
      pair_list<TKey, TValue>& other
      );

    //
    // element access.
    //

    TValue&
    operator[](
      const TKey& key
      );

    TValue&
    first_value(
      void
      );

    const TValue&
    first_value(
      void
      ) const;

    TValue&
    last_value(
      void
      );

    const TValue&
    last_value(
      void
      ) const;

    //
    // iterators.
    //

    pair<TKey, TValue>*
    begin(
      void
      );

    const pair<TKey, TValue>*
    begin(
      void
      ) const;

    pair<TKey, TValue>*
    end(
      void
      );

    const pair<TKey, TValue>*
    end(
      void
      ) const;

    //
    // capacity.
    //

    bool
    is_empty(
      void
      ) const;

    size_type
    get_size(
      void
      ) const;

    size_type
    get_capacity(
      void
      ) const;

    //
    // lookup.
    //

    bool
    contains(
      const TKey& key
      ) const;

    TValue*
    find(
      const TKey& key
      );

    const TValue*
    find(
      const TKey& key
      ) const;

    pair<TKey, TValue>*
    find_pair(
      const TKey& key
      );

    //
    // modifiers.
    //

    //
    // both TKey and TValue must be copy-constructible.
    //
    pair<TKey, TValue>&
    insert(
      const TKey& key,
      const TValue& value
      );

    pair<TKey, TValue>&
    insert(
      TKey&& key,
      TValue&& value
      );

    pair<TKey, TValue>&
    insert(
      const pair<TKey, TValue>& pair
      );

    pair<TKey, TValue>&
    insert(
      pair<TKey, TValue>&& pair
      );

    void
    remove(
      const TKey& key
      );

    void
    reserve(
      size_type new_capacity
      );

    void
    clear(
      void
      );

  private:
    list<pair<TKey, TValue>> _buffer;
};

}

#include "pair_list.inl"
