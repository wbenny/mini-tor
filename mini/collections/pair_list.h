#pragma once
#include <mini/pair.h>
#include <mini/collections/list.h>

namespace mini::collections {

template <
  typename TKEY,
  typename TVALUE
>
class pair_list
{
  public:
    pair_list(
      void
      );

    pair_list(
      const pair_list& other
      );

    pair_list(
      pair_list&& other
      );

    ~pair_list(
      void
      );

    TVALUE&
    operator[](
      const TKEY& key
      );

    //
    // both TKEY and TVALUE must be copy-constructible.
    //
    pair<TKEY, TVALUE>&
    insert(
      const TKEY& key,
      const TVALUE& value
      );

    pair<TKEY, TVALUE>&
    insert(
      TKEY&& key,
      TVALUE&& value
      );

    pair<TKEY, TVALUE>&
    insert(
      const pair<TKEY, TVALUE>& pair
      );

    pair<TKEY, TVALUE>&
    insert(
      pair<TKEY, TVALUE>&& pair
      );

    pair<TKEY, TVALUE>*
    find_pair(
      const TKEY& key
      );

    TVALUE*
    find(
      const TKEY& key
      );

    const TVALUE*
    find(
      const TKEY& key
      ) const;

    void
    remove(
      const TKEY& key
      );

    void
    reserve(
      size_type new_capacity
      );

    void
    clear(
      void
      );

    TVALUE&
    first_value(
      void
      );

    const TVALUE&
    first_value(
      void
      ) const;

    TVALUE&
    last_value(
      void
      );

    const TVALUE&
    last_value(
      void
      ) const;

    pair<TKEY, TVALUE>*
    begin(
      void
      );

    const pair<TKEY, TVALUE>*
    begin(
      void
      ) const;

    pair<TKEY, TVALUE>*
    end(
      void
      );

    const pair<TKEY, TVALUE>*
    end(
      void
      ) const;

    bool
    is_empty(
      void
      ) const;

    bool
    contains(
      const TKEY& key
      ) const;

    void
    swap(
      pair_list<TKEY, TVALUE>& other
      );

    size_type
    get_size(
      void
      ) const;

    size_type
    get_capacity(
      void
      ) const;

  private:
    list<pair<TKEY, TVALUE>> _buffer;
};

}

#include "pair_list.inl"
