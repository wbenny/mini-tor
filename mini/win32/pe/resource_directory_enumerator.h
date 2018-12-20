#pragma once
#include "common.h"
#include "../../common.h"
#include "../../byte_buffer_ref.h"

#include <iterator>

#include <cstdint>

namespace mini::win32 {

template <
  typename TImageTraits
>
class pe_resource_directory_enumerator
{
  friend class pe_file;

  public:
    struct proxy
    {
      friend class pe_resource_directory_enumerator;

      struct resource_string
      {
        wchar_t* buffer;
        size_t   size;
      };

      struct resource_data
      {
        byte_buffer_ref buffer;
        uint32_t code_page;
      };

      bool
      name_is_string(
        void
        ) const
      {
        return !!_resource_directory_entry->name_is_string;
      }

      resource_string
      name(
        void
        ) const
      {
        auto resource_dir_string_u = reinterpret_cast<typename TImageTraits::image_resource_dir_string_u_t*>(
          uintptr_t(_top_resource_directory.entry) + _resource_directory_entry->name_offset
          );

        return {
          resource_dir_string_u->name_string,
          resource_dir_string_u->length * sizeof(wchar_t)
        };
      }

      uint16_t
      id(
        void
        ) const
      {
        return _resource_directory_entry->id;
      }

      bool
      data_is_directory(
        void
        ) const
      {
        return !!_resource_directory_entry->data_is_directory;
      }

      resource_data
      data(
        void
        ) const
      {
        auto resource_data_entry = reinterpret_cast<typename TImageTraits::image_resource_data_entry_t*>(
          uintptr_t(_top_resource_directory.entry) + _resource_directory_entry->offset_to_data
          );

        auto buffer_begin = reinterpret_cast<uint8_t*>(uintptr_t(_top_resource_directory.entry) + resource_data_entry->offset_to_data);
        auto buffer_end   = buffer_begin + resource_data_entry->size;

        return {
          byte_buffer_ref(
            buffer_begin,
            buffer_end
          ),
          resource_data_entry->code_page
        };
      }

      pe_resource_directory_enumerator
      resource_directory(
        void
        ) const
      {
        auto nested_resource_directory = reinterpret_cast<typename TImageTraits::image_resource_directory_t*>(
          uintptr_t(_top_resource_directory.entry) + _resource_directory_entry->offset_to_directory
          );

        return pe_resource_directory_enumerator(
          _pe,
          { nested_resource_directory, 1 }, // fake size (1)
          _top_resource_directory
        );
      }

    private:
      const pe_file* _pe;
      pe_directory<typename TImageTraits::image_resource_directory_t> _top_resource_directory;
      typename TImageTraits::image_resource_directory_entry_t* _resource_directory_entry;
    };

    struct iterator
    {
      friend class pe_resource_directory_enumerator;

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
          const pe_resource_directory_enumerator* enumerator,
          size_t index = 0
          );

        value_type value() const;

        const pe_resource_directory_enumerator* _enumerator;
        size_t    _index;

        typename TImageTraits::image_resource_directory_entry_t* _resource_entry_table;
    };

    pe_resource_directory_enumerator() = delete;
    pe_resource_directory_enumerator(const pe_resource_directory_enumerator& other) = default;
    pe_resource_directory_enumerator(pe_resource_directory_enumerator&& other) = default;
    pe_resource_directory_enumerator& operator=(const pe_resource_directory_enumerator& other) = default;
    pe_resource_directory_enumerator& operator=(pe_resource_directory_enumerator&& other) = default;
    ~pe_resource_directory_enumerator() = default;

    iterator begin() const { return iterator(this);         }
    iterator end()   const { return iterator(this, _count); }
    size_t   count() const { return _count;                 }
    size_t   size()  const { return _count;                 }

    uint32_t characteristics() const;

  private:
    pe_resource_directory_enumerator(
      const pe_file* pe
      );

    pe_resource_directory_enumerator(
      const pe_file* pe,
      pe_directory<typename TImageTraits::image_resource_directory_t> resource_directory,
      pe_directory<typename TImageTraits::image_resource_directory_t> top_resource_directory
    );

    const pe_file* _pe;
    pe_directory<typename TImageTraits::image_resource_directory_t> _resource_directory;
    size_t _count;

    pe_directory<typename TImageTraits::image_resource_directory_t> _top_resource_directory;
};


}
