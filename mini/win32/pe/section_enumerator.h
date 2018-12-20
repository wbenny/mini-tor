#pragma once
#include "common.h"
#include "../../common.h"

#include <iterator>

#include <cstdint>

namespace mini::win32 {

template <
  typename TImageTraits
>
class pe_section_enumerator
{
  friend class pe_file;

  public:
    struct proxy
    {
      friend class pe_section_enumerator;

      char*
      name(
        void
        ) const
      {
        return reinterpret_cast<char*>(_section_header->name);
      }

      uint32_t
      characteristics(
        void
        ) const
      {
        return _section_header->characteristics;
      }

    private:
      uint8_t* _image_base;
      typename TImageTraits::image_section_header_t* _section_header;
    };

    struct iterator
    {
      friend class pe_section_enumerator;

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
        iterator(
          const pe_section_enumerator* enumerator,
          size_t index = 0
          );

        value_type value() const;

        const pe_section_enumerator* _enumerator;
        size_t _index;
    };

    pe_section_enumerator() = delete;
    pe_section_enumerator(const pe_section_enumerator& other) = default;
    pe_section_enumerator(pe_section_enumerator&& other) = default;
    pe_section_enumerator& operator=(const pe_section_enumerator& other) = default;
    pe_section_enumerator& operator=(pe_section_enumerator&& other) = default;
    ~pe_section_enumerator() = default;

    iterator begin() const { return iterator(this);         }
    iterator end()   const { return iterator(this, _count); }
    size_t   count() const { return _count;                 }
    size_t   size()  const { return _count;                 }

  private:
    pe_section_enumerator(
      const pe_file* pe
      );

    const pe_file* _pe;
    typename TImageTraits::image_section_header_t* _section_table;
    size_t _count;
};

}
