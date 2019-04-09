#pragma once
#include "common.h"

#include <type_traits>

namespace mini {

namespace detail {

  //
  // Kindly borrowed from the MSVC CRT.
  //

#if defined(MINI_ARCH_X86) || defined(MINI_ARCH_ARM32)
  inline constexpr size_type FNV_offset_basis = 0x811C9DC5u;
  inline constexpr size_type FNV_prime        = 0x1000193u;
#elif defined(MINI_ARCH_X64) || defined(MINI_ARCH_ARM64)
  inline constexpr size_type FNV_offset_basis = 0xCBF29CE484222325ull;
  inline constexpr size_type FNV_prime        = 0x100000001B3ull;
#endif

  inline size_type
  fnv1a_append_bytes(
    size_type value,
    const unsigned char* const first,
    const size_type count
    ) noexcept
  {
    for (size_type index = 0; index < count; ++index)
    {
      value ^= static_cast<size_type>(first[index]);
      value *= FNV_prime;
    }

    return (value);
  }

  template<
    typename T
  >
  inline size_type
  fnv1a_append_range(
    const size_type value,
    const T* const first,
    const T* const last
    ) noexcept
  {
    static_assert(std::is_trivial_v<T>, "Only trivial types can be directly hashed.");
    const auto firstb = reinterpret_cast<const unsigned char*>(first);
    const auto lastb  = reinterpret_cast<const unsigned char*>(last);
    return (fnv1a_append_bytes(value, firstb, static_cast<size_type>(lastb - firstb)));
  }

  template<
    typename T
  >
  inline size_type
  fnv1a_append_value(
    const size_type value,
    const T& key_value
    ) noexcept
  {
    static_assert(std::is_trivial_v<T>, "Only trivial types can be directly hashed.");
    return (fnv1a_append_bytes(
      value,
      &reinterpret_cast<const unsigned char&>(key_value),
      sizeof(T))
      );
  }

  template<
    typename T
  >
  inline size_type
  hash_representation(
    const T& key_value
    ) noexcept
  {
    return (fnv1a_append_value(FNV_offset_basis, key_value));
  }

  template<
    typename T
  >
  inline size_type
  hash_array_representation(
    const T* const first,
    const size_type count
    ) noexcept
  {
    static_assert(std::is_trivial_v<T>, "Only trivial types can be directly hashed.");
    return (fnv1a_append_bytes(
      FNV_offset_basis,
      reinterpret_cast<const unsigned char *>(first),
      count * sizeof(T))
      );
  }
}

template <
  typename T
>
struct hash
  : std::hash<T>
{

};

}
