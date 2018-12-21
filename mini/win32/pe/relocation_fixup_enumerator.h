#pragma once
#include "common.h"
#include "../../common.h"

#include <iterator>

#include <cstdint>

namespace mini::win32 {

template <
  typename TImageTraits
>
class pe_relocation_fixup_enumerator
{
  friend class pe_file;
  template <typename> friend class pe_relocation_directory_enumerator;

  public:
    struct proxy
    {
      friend class pe_relocation_fixup_enumerator;

      void*
      virtual_address(
        void
        ) const
      {
        return _image_base + relative_virtual_address();
      }

      uintptr_t
      relative_virtual_address(
        void
        ) const
      {
        return _relocation_base + (*_fixup_item & 0xfff);
      }

      uint16_t
      type(
        void
        ) const
      {
        return *_fixup_item >> 12;
      }

    private:
      uint8_t* _image_base;
      uintptr_t _relocation_base;
      uint16_t* _fixup_item;
    };

    struct iterator
    {
      friend class pe_relocation_fixup_enumerator;

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
          const pe_relocation_fixup_enumerator* enumerator,
          size_t index = 0
          );

        value_type value() const;

        const pe_relocation_fixup_enumerator* _enumerator;
        size_t _index;
    };

    pe_relocation_fixup_enumerator() = delete;
    pe_relocation_fixup_enumerator(const pe_relocation_fixup_enumerator& other) = default;
    pe_relocation_fixup_enumerator(pe_relocation_fixup_enumerator&& other) = default;
    pe_relocation_fixup_enumerator& operator=(const pe_relocation_fixup_enumerator& other) = default;
    pe_relocation_fixup_enumerator& operator=(pe_relocation_fixup_enumerator&& other) = default;
    ~pe_relocation_fixup_enumerator() = default;

    iterator begin() const { return iterator(this);         }
    iterator end()   const { return iterator(this, _count); }
    size_t   count() const { return _count;                 }
    size_t   size()  const { return _count;                 }

  private:
    pe_relocation_fixup_enumerator(
      uint8_t* image_base,
      typename TImageTraits::image_base_relocation_t* relocation
      );

    uint8_t* _image_base;
    typename TImageTraits::image_base_relocation_t* _relocation;
    size_t _count;
};

}
