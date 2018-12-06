#pragma once
#include <mini/buffer_ref.h>
#include <mini/hash.h>

namespace mini {

using         byte_buffer_ref =         buffer_ref<byte_type>;
using mutable_byte_buffer_ref = mutable_buffer_ref<byte_type>;

template<>
struct hash<byte_buffer_ref>
{
  size_type operator()(const byte_buffer_ref& value) const noexcept
  {
    return detail::hash_array_representation(value.get_buffer(), value.get_size());
  }
};

template<>
struct hash<mutable_byte_buffer_ref>
{
  size_type operator()(const mutable_byte_buffer_ref& value) const noexcept
  {
    return detail::hash_array_representation(value.get_buffer(), value.get_size());
  }
};

}

