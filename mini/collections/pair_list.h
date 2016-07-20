#pragma once
#include <mini/collections/list.h>
#include <mini/pair.h>

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

    template <
      typename = std::enable_if_t<std::is_copy_constructible_v<TKEY> && std::is_copy_constructible_v<TVALUE>>
    >
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
      size_t new_capacity
      );

    void clear(
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

    size_t
    get_size(
      void
      ) const;

    size_t
    get_capacity(
      void
      ) const;

  private:
    list<pair<TKEY, TVALUE>> _buffer;
};

}

#include "pair_list.inl"
