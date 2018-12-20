#pragma once
#include "relocation_fixup_enumerator.h"

#include "common.h"
#include "../../common.h"

#include <iterator>

#include <cstdint>

namespace mini::win32 {

template <
  typename TImageTraits
>
class pe_relocation_directory_enumerator
{
  friend class pe_file;

  public:
    struct proxy
    {
      friend class pe_relocation_directory_enumerator;

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
        return _relocation->virtual_address;
      }

      pe_relocation_fixup_enumerator<TImageTraits>
      fixups(
        void
        ) const
      {
        return pe_relocation_fixup_enumerator<TImageTraits>(_image_base, _relocation);
      }

    private:
      uint8_t* _image_base;
      image_base_relocation_t* _relocation;
    };

    struct iterator
    {
      friend class pe_relocation_directory_enumerator;

      using iterator_category = std::forward_iterator_tag;

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

      iterator& operator++(   )                               {                   advance(); return *this; }
      iterator  operator++(int)                               { auto tmp = *this; advance(); return tmp;   }

      bool      operator==(const iterator& other)       const { return _relocation_current == other._relocation_current && _enumerator == other._enumerator; }
      bool      operator!=(const iterator& other)       const { return _relocation_current != other._relocation_current || _enumerator != other._enumerator; }

      reference operator*()                             const { return value(); }
      pointer   operator->()                            const { return value(); }

      private:
        explicit iterator(
          const pe_relocation_directory_enumerator* enumerator,
          typename TImageTraits::image_base_relocation_t* current_relocation
          );

        void       advance();
        value_type value() const;

        const pe_relocation_directory_enumerator* _enumerator;
        typename TImageTraits::image_base_relocation_t* _relocation_current;
        typename TImageTraits::image_base_relocation_t* _relocation_end;
    };

    pe_relocation_directory_enumerator() = delete;
    pe_relocation_directory_enumerator(const pe_relocation_directory_enumerator& other) = default;
    pe_relocation_directory_enumerator(pe_relocation_directory_enumerator&& other) = default;
    pe_relocation_directory_enumerator& operator=(const pe_relocation_directory_enumerator& other) = default;
    pe_relocation_directory_enumerator& operator=(pe_relocation_directory_enumerator&& other) = default;
    ~pe_relocation_directory_enumerator() = default;

    iterator begin() const { return iterator(this, _base_relocation.entry); }
    iterator end()   const { return iterator(this, _base_relocation_end);   }

  private:
    pe_relocation_directory_enumerator(
      const pe_file* pe
      );

    const pe_file* _pe;
    pe_directory<typename TImageTraits::image_base_relocation_t> _base_relocation;
    typename TImageTraits::image_base_relocation_t* _base_relocation_end;
};


}
