#pragma once
#include "common.h"
#include "../../common.h"

#include <iterator>

#include <cstdint>

namespace mini::win32 {

template <
  typename TImageTraits
>
class pe_import_thunk_enumerator
{
  friend class pe_file;
  template <typename> friend class pe_import_directory_enumerator;

  public:
    struct proxy
    {
      friend class pe_import_thunk_enumerator;

      char*
      name(
        void
        ) const
      {
        auto import_by_name = reinterpret_cast<typename TImageTraits::image_import_by_name_t*>(_image_base + _name_thunk_data->address_of_data);

        return reinterpret_cast<char*>(import_by_name->name);
      }

      uint16_t
      hint(
        void
        ) const
      {
        auto import_by_name = reinterpret_cast<typename TImageTraits::image_import_by_name_t*>(_image_base + _name_thunk_data->address_of_data);

        return import_by_name->hint;
      }

      uint16_t
      ordinal(
        void
        ) const
      {
        return static_cast<uint16_t>(_name_thunk_data->ordinal);
      }

      bool
      is_ordinal(
        void
        ) const
      {
        return !!(_name_thunk_data->ordinal & TImageTraits::image_ordinal_flag);
      }

    private:
      uint8_t* _image_base;
      typename TImageTraits::image_thunk_data_t* _name_thunk_data;
    };

    struct iterator
    {
      friend class pe_import_thunk_enumerator;

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
          const pe_import_thunk_enumerator* enumerator,
          size_t index = 0
          );

        value_type value() const;

        const pe_import_thunk_enumerator* _enumerator;
        size_t    _index;
    };

    pe_import_thunk_enumerator() = delete;
    pe_import_thunk_enumerator(const pe_import_thunk_enumerator& other) = default;
    pe_import_thunk_enumerator(pe_import_thunk_enumerator&& other) = default;
    pe_import_thunk_enumerator& operator=(const pe_import_thunk_enumerator& other) = default;
    pe_import_thunk_enumerator& operator=(pe_import_thunk_enumerator&& other) = default;
    ~pe_import_thunk_enumerator() = default;

    iterator begin() const { return iterator(this);         }
    iterator end()   const { return iterator(this, _count); }
    size_t   count() const { return _count;                 }
    size_t   size()  const { return _count;                 }

  private:
    pe_import_thunk_enumerator(
      uint8_t* image_base,
      typename TImageTraits::image_import_descriptor_t* import_descriptor
      );

    uint8_t* _image_base;
    typename TImageTraits::image_thunk_data_t* _name_thunk_data;
    typename TImageTraits::image_thunk_data_t* _address_thunk_data;
    size_t _count;
};

}


