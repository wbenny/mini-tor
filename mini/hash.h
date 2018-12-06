#pragma once
#include "common.h"

#include <type_traits>

namespace mini {

namespace detail {

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
    const auto firstb = reinterpret_cast<const unsigned char *>(first);
    const auto lastb = reinterpret_cast<const unsigned char *>(last);
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
struct hash;

template <
  typename T,
  bool Enabled
>
struct hash_numeric
{
  size_type operator()(const T& value) const noexcept
  {
    return detail::hash_representation(value);
  }
};

template <
  typename T
>
struct hash_numeric<T, false>
{
  hash_numeric() = delete;
  hash_numeric(const hash_numeric&) = delete;
  hash_numeric(hash_numeric&&) = delete;
  hash_numeric& operator=(const hash_numeric&) = delete;
  hash_numeric& operator=(hash_numeric&&) = delete;
};

template <
  typename T
>
struct hash
  : hash_numeric<
      T,
      !std::is_const_v<T> && !std::is_volatile_v<T> &&
      (std::is_enum_v<T> || std::is_integral_v<T> || std::is_pointer_v<T>)
    >
{

};

template <>
struct hash<float>
{
  size_type operator()(const float value) const noexcept
  {
    return detail::hash_representation(value == 0.0f ? 0.0f : value); // map -0 to 0
  }
};

template <>
struct hash<double>
{
  size_type operator()(const double value) const noexcept
  {
    return detail::hash_representation(value == 0.0f ? 0.0f : value); // map -0 to 0
  }
};

template <>
struct hash<long double>
{
  size_type operator()(const long double value) const noexcept
  {
    return detail::hash_representation(value == 0.0f ? 0.0f : value); // map -0 to 0
  }
};

}
