#pragma once
#include "common.h"
#include "../../common.h"

#include "api_set_value_enumerator.h"

#include <iterator>

namespace mini::win32 {

class api_set_enumerator
{
  friend class api_set_t;

  public:
    struct proxy
    {
      friend class api_set_enumerator;

      struct api_set_string
      {
        wchar_t* buffer;
        size_t   size;
      };

      api_set_string
      name(
        void
        ) const
      {
        return { _name, _name_length };
      }

      api_set_value_enumerator
      values() const
      {
        return api_set_value_enumerator(_api_set_namespace_base, _api_set_value_table, _api_set_value_count);
      }

    private:
      wchar_t* _name;
      size_t _name_length;

      uint8_t* _api_set_namespace_base;
      api_set_value_entry_t* _api_set_value_table;
      size_t _api_set_value_count;
    };

    struct iterator
    {
      friend class api_set_enumerator;

      using iterator_category = std::bidirectional_iterator_tag;

      using value_type        = proxy;
      using difference_type   = pointer_difference_type;
      using pointer           = value_type;
      using reference         = value_type;

      iterator() = default;
      iterator(const iterator& other) = default;
      iterator(iterator&& other) = default;
      iterator& operator=(const iterator& other) = default;
      iterator& operator=(iterator&& other) = default;
      ~iterator() = default;

      iterator& operator++(   )                               {                   ++_index; return *this; }
      iterator  operator++(int)                               { auto tmp = *this; ++_index; return tmp;   }
      iterator& operator--(   )                               {                   --_index; return *this; }
      iterator  operator--(int)                               { auto tmp = *this; --_index; return tmp;   }

      bool      operator==(const iterator& other)       const { return _index == other._index && _enumerator == other._enumerator; }
      bool      operator!=(const iterator& other)       const { return _index != other._index || _enumerator != other._enumerator; }

      reference operator*()                             const { return value(); }
      pointer   operator->()                            const { return value(); }

      private:
        explicit iterator(
          const api_set_enumerator* enumerator,
          size_t index = 0
          );

        value_type value() const;

        const api_set_enumerator* _enumerator;
        size_t    _index;
    };

    api_set_enumerator() = delete;
    api_set_enumerator(const api_set_enumerator& other) = default;
    api_set_enumerator(api_set_enumerator&& other) = default;
    api_set_enumerator& operator=(const api_set_enumerator& other) = default;
    api_set_enumerator& operator=(api_set_enumerator&& other) = default;
    ~api_set_enumerator() = default;

    iterator begin() const { return iterator(this);         }
    iterator end()   const { return iterator(this, _count); }
    size_t   count() const { return _count;                 }
    size_t   size()  const { return _count;                 }

  private:
    api_set_enumerator(
      const api_set_t* api_set
      );

    const api_set_t* _api_set;
    size_t _count;
};

}
