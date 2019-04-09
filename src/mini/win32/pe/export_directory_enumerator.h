#pragma once
#include "common.h"
#include "../../common.h"

#include <iterator>

#include <cstdint>

namespace mini::win32 {

template <
  typename TImageTraits
>
class pe_export_directory_enumerator
{
  friend class pe_file;

  public:
    struct proxy
    {
      friend class pe_export_directory_enumerator;

      char*
      name(
        void
        ) const
      {
        return reinterpret_cast<char*>(_image_base + _name);
      }

      uint16_t
      ordinal(
        void
        ) const
      {
        return _ordinal;
      }

      bool
      is_forwarder(
        void
        ) const
      {
        return function() >= _export_directory.entry
            && function() <= _export_directory.entry + _export_directory.size;
      }

      void*
      function(
        void
        ) const
      {
        return reinterpret_cast<void*>(_image_base + _function);
      }

      char*
      forwarder(
        void
        ) const
      {
        return reinterpret_cast<char*>(function());
      }

    private:
      uint8_t* _image_base;
      pe_directory<typename TImageTraits::image_export_directory_t> _export_directory;
      uint32_t _function;
      uint32_t _name;
      uint16_t _ordinal;
    };

    struct iterator
    {
      friend class pe_export_directory_enumerator;

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
          const pe_export_directory_enumerator* enumerator,
          size_t index = 0
          );

        value_type value() const;

        const pe_export_directory_enumerator* _enumerator;
        size_t    _index;

        uint32_t* _function_table;
        uint32_t* _name_table;
        uint16_t* _ordinal_table;
    };

    pe_export_directory_enumerator() = delete;
    pe_export_directory_enumerator(const pe_export_directory_enumerator& other) = default;
    pe_export_directory_enumerator(pe_export_directory_enumerator&& other) = default;
    pe_export_directory_enumerator& operator=(const pe_export_directory_enumerator& other) = default;
    pe_export_directory_enumerator& operator=(pe_export_directory_enumerator&& other) = default;
    ~pe_export_directory_enumerator() = default;

    iterator begin() const { return iterator(this);         }
    iterator end()   const { return iterator(this, _count); }
    size_t   count() const { return _count;                 }
    size_t   size()  const { return _count;                 }

    uint32_t characteristics() const;

  private:
    pe_export_directory_enumerator(
      const pe_file* pe
      );

    const pe_file* _pe;
    pe_directory<typename TImageTraits::image_export_directory_t> _export_directory;
    size_t _count;
};

}
