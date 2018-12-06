#pragma once
#include <mini/collections/list.h>
#include <mini/hash.h>

namespace mini {

using byte_buffer = collections::list<byte_type>;

template<>
struct hash<byte_buffer>
{
  size_type operator()(const byte_buffer& value) const noexcept
  {
    return detail::hash_array_representation(value.get_buffer(), value.get_size());
  }
};

}
