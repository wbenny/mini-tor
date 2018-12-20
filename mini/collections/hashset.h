#pragma once
#include <mini/pair.h>
#include <mini/hash.h>
#include <mini/compare.h>
#include <mini/collections/list.h>

#include <iterator>

namespace mini::collections {

template <
  typename T,
  typename IndexType = int,
  typename Hash = hash<T>,
  typename KeyEqual = equal_to<T>,
  typename Allocator = allocator<T>
>
class hashset
{
  public:
    using value_type              = T;
    using size_type               = size_type;
    using difference_type         = pointer_difference_type;

    using hasher                  = Hash;
    using key_equal               = KeyEqual;
    using allocator_type          = Allocator;

    using pointer                 = value_type*;
    using const_pointer           = const value_type*;
    using reference               = value_type&;
    using const_reference         = const value_type&;

    using index_type              = IndexType;

    static constexpr index_type invalid_index = (index_type)-1;

  private:
    struct node_type
    {
      value_type item;
      index_type next;
    };

  public:
    struct iterator
    {
      friend class hashset;

      using iterator_category = std::bidirectional_iterator_tag;

      using value_type        = T;
      using difference_type   = pointer_difference_type;
      using pointer           = value_type*;
      using reference         = value_type&;

      iterator& operator++(   )                               {                   ++_node; return *this; }
      iterator  operator++(int)                               { auto tmp = *this; ++_node; return tmp;   }
      iterator& operator--(   )                               {                   --_node; return *this; }
      iterator  operator--(int)                               { auto tmp = *this; --_node; return tmp;   }

      bool      operator==(const iterator& other)       const { return _node == other._node; }
      bool      operator!=(const iterator& other)       const { return _node != other._node; }

      reference operator*()                             const { return  _node->item; }
      pointer   operator->()                                  { return &_node->item; }

      private:
        iterator(
          node_type* node
          ) : _node(node) { }

        node_type* _node;
    };

    struct const_iterator
    {
      friend class hashset;

      using iterator_category = std::bidirectional_iterator_tag;

      using value_type        = T;
      using difference_type   = pointer_difference_type;
      using pointer           = const value_type*;
      using reference         = const value_type&;

      const_iterator& operator++(   )                         {                   ++_node; return *this; }
      const_iterator  operator++(int)                         { auto tmp = *this; ++_node; return tmp;   }
      const_iterator& operator--(   )                         {                   --_node; return *this; }
      const_iterator  operator--(int)                         { auto tmp = *this; --_node; return tmp;   }

      bool      operator==(const const_iterator& other) const { return _node == other._node; }
      bool      operator!=(const const_iterator& other) const { return _node != other._node; }

      reference operator*()                             const { return  _node->item; }
      pointer   operator->()                            const { return &_node->item; }

      private:
        const_iterator(
          const node_type* node
          ) : _node(node) { }

        const node_type* _node;
    };

    //
    // constructors.
    //

    hashset(
      void
      );

    hashset(
      const hashset& other
      );

    hashset(
      hashset&& other
      );

    hashset(
      std::initializer_list<T> values
      );

    hashset(
      size_type reserve_size
      );

    //
    // destructor.
    //

    ~hashset(
      void
      );

    //
    // assign operators.
    //

    hashset&
    operator=(
      const hashset& other
      );

    hashset&
    operator=(
      hashset&& other
      );

    //
    // swap.
    //

    void
    swap(
      hashset& other
      );

    //
    // iterators.
    //

    iterator
    begin(
      void
      );

    const_iterator
    begin(
      void
      ) const;

    iterator
    end(
      void
      );

    const_iterator
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
    get_bucket_count(
      void
      );

    void
    reserve(
      size_type new_capacity
      );

    //
    // lookup.
    //

    bool
    contains(
      const T& item
      ) const;

    iterator
    find(
      const T& item
      );

    const_iterator
    find(
      const T& item
      ) const;

    index_type
    get_bucket(
      const T& item
      ) const;

    //
    // modifiers.
    //

    iterator
    insert(
      const T& item
      );

    iterator
    insert_many(
      std::initializer_list<T> values
      );

    iterator
    remove(
      iterator it
      );


    void
    remove(
      const value_type& item
      );

    void
    clear(
      void
      );

  protected:
    template <
      typename U
    >
    iterator
    find_generic(
      const U& item
      );

    template <
      typename U
    >
    const_iterator
    find_generic(
      const U& item
      ) const;

    template <
      typename U
    >
    index_type
    get_bucket_generic(
      const U& item
      ) const;

  private:
    using entry_allocator = typename Allocator::template rebind<node_type>::other;
    using index_allocator = typename Allocator::template rebind<index_type>::other;

    void
    rehash(
      void
      );

    void
    unbind_entry(
      size_type bucket,
      index_type index
      );

    list<node_type>  _node_list;
    list<index_type> _bucket_list;
    hasher           _hasher;
    key_equal        _equal;
};

}

namespace mini {

//
// non-class functions.
//

template <
  typename T,
  typename IndexType,
  typename Hash,
  typename KeyEqual,
  typename Allocator
>
void
swap(
  mini::collections::hashset<T, IndexType, Hash, KeyEqual, Allocator>& lhs,
  mini::collections::hashset<T, IndexType, Hash, KeyEqual, Allocator>& rhs
  );

}

#include "hashset.inl"
