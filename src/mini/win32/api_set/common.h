#pragma once
#include <cstdint>

namespace mini::win32 {

struct api_set_namespace_t
{
  uint32_t version;
  uint32_t size;
  uint32_t flags;
  uint32_t count;
  uint32_t entry_offset;
  uint32_t hash_offset;
  uint32_t hash_factor;
};

struct api_set_hash_entry_t
{
  uint32_t hash;
  uint32_t index;
};

struct api_set_namespace_entry_t
{
  uint32_t flags;
  uint32_t name_offset;
  uint32_t name_length;
  uint32_t hashed_length;
  uint32_t value_offset;
  uint32_t value_count;
};

struct api_set_value_entry_t
{
  uint32_t flags;
  uint32_t name_offset;
  uint32_t name_length;
  uint32_t value_offset;
  uint32_t value_length;
};

}
