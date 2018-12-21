#pragma once
#include "api_set/api_set_enumerator.h"

#include "api_set/common.h"

#include <mini/string_ref.h>

namespace mini::win32 {

class api_set_t
{
  friend class api_set_enumerator;

  public:
    api_set_t(
      api_set_namespace_t* api_set_namespace
      )
      : _api_set_namespace(api_set_namespace)
    {

    }

    api_set_enumerator
    enumerator(
      void
      ) const
    {
      return api_set_enumerator(this);
    }

    api_set_value_enumerator
    find(
      string_ref api_set_name
      ) const
    {
      auto apiset_entry_count = _api_set_namespace->count - 1;

      //
      // Sanity checks.
      //

      if ((apiset_entry_count) < 0 ||
          !api_set_name.get_size())
      {
        return api_set_value_enumerator(nullptr, nullptr, 0);
      }

      //
      // Create hash of the ApiSet name.
      //

      uint32_t hashkey = hash(api_set_name);

      //
      // Find the hash in the hashmap.
      //

      uint32_t hash_counter = 0;
      uint32_t hash_index = 0;
      uint32_t hash_offset = 0;
      api_set_hash_entry_t* entry = nullptr;

      while (hash_counter <= apiset_entry_count)
      {
        hash_index  = (apiset_entry_count + hash_counter) / 2;
        hash_offset = _api_set_namespace->hash_offset
                    + hash_index * sizeof(uint64_t); // 64-bit ptr

        auto current_entry = reinterpret_cast<api_set_hash_entry_t*>(_api_set_namespace_base + hash_offset);

        if (hashkey < current_entry->hash)
        {
          apiset_entry_count = hash_index - 1;
        }
        else if (hashkey > current_entry->hash)
        {
          hash_counter = hash_index + 1;
        }
        else // if (hashkey == current_entry->hash)
        {
          entry = current_entry;
          break;
        }
      }

      if (!entry)
      {
        return api_set_value_enumerator(nullptr, nullptr, 0);
      }

      //
      // Get the corresponding hash bucket value.
      //

      auto namespace_entry = &reinterpret_cast<api_set_namespace_entry_t*>(
        _api_set_namespace_base + _api_set_namespace->entry_offset
        )[entry->index];

      auto result_name = reinterpret_cast<wchar_t*>(_api_set_namespace_base + namespace_entry->name_offset);

      //
      // Check the returned hash entry actually correspond to the given ApiSet name.
      //

      if (api_set_name.get_size() != namespace_entry->hashed_length / sizeof(wchar_t) ||
          !wcsmbseq(api_set_name.get_buffer(), result_name, api_set_name.get_size()))
      {
        return api_set_value_enumerator(nullptr, nullptr, 0);
      }

      auto value_table = reinterpret_cast<api_set_value_entry_t*>(
        _api_set_namespace_base + namespace_entry->value_offset
        );

      return api_set_value_enumerator(_api_set_namespace_base, value_table, namespace_entry->value_count);
    }

    static string_ref
    dll_to_api_set_name(
      string_ref dll_name
      )
    {
      size_t last_index_of_hyphen;

      if (!is_api_set_name(dll_name) ||
          !dll_name.ends_with(".dll") ||
          (last_index_of_hyphen = dll_name.last_index_of('-')) == string_ref::not_found)
      {
        return string_ref::empty;
      }

      //
      // Strip version + '.dll' extension.
      //

      return string_ref(dll_name.begin(), dll_name.begin() + last_index_of_hyphen);
    }

    static bool
    is_api_set_name(
      string_ref api_set_name
      )
    {
      return api_set_name.starts_with("api-")
          || api_set_name.starts_with("ext-");
    }

  private:
    uint32_t
    hash(
      string_ref api_set_name
      ) const
    {
      auto api_set_name_buffer = api_set_name.get_buffer();
      auto api_set_name_length = api_set_name.get_size();

      uint32_t result = 0;

      do
      {
        auto c = (*api_set_name_buffer - 'A') <= 0x19u
          ? *api_set_name_buffer + ' '
          : *api_set_name_buffer;

        result *= _api_set_namespace->hash_factor;
        result += c;

      } while (--api_set_name_length && ++api_set_name_buffer);

      return result;
    }

    bool
    wcsmbseq(
      const char* string1,
      const wchar_t* string2,
      size_t max_count
      ) const
    {
      while (max_count--)
      {
        //
        // Check if high bytes of the string2 are 0 (i.e. the character is ASCII).
        // Check if low bytes of the string2 matches those in string1.
        //

        if (
          (*string2 & 0xFF00) ||
           *string1++ != char(*string2++))
        {
          return false;
        }
      }

      return true;
    }

    union
    {
      api_set_namespace_t* _api_set_namespace;
      uint8_t* _api_set_namespace_base;
    };
};

}
