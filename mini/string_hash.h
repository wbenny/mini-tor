#pragma once
#include <mini/string_ref.h>

namespace mini {

namespace detail {

//
// ref: https://gist.github.com/filsinger/1255697
//

  template <typename S> struct fnv_internal;
  template <typename S> struct fnv1;
  template <typename S> struct fnv1a;

  template <> struct fnv_internal<uint32_t>
  {
    constexpr static uint32_t default_offset_basis = 0x811C9DC5;
    constexpr static uint32_t prime = 0x01000193;
  };

  template <> struct fnv1<uint32_t> : public fnv_internal<uint32_t>
  {
    constexpr static inline uint32_t hash(char const*const aString, const uint32_t val = default_offset_basis)
    {
      return (aString[0] == '\0') ? val : hash(&aString[1], (val * prime) ^ uint32_t(aString[0]));
    }

    constexpr static inline uint32_t hash(char const*const aString, const size_t aStrlen, const uint32_t val)
    {
      return (aStrlen == 0) ? val : hash(aString + 1, aStrlen - 1, (val * prime) ^ uint32_t(aString[0]));
    }
  };

  template <> struct fnv1a<uint32_t> : public fnv_internal<uint32_t>
  {
    constexpr static inline uint32_t hash(char const*const aString, const uint32_t val = default_offset_basis)
    {
      return (aString[0] == '\0') ? val : hash(&aString[1], (val ^ uint32_t(aString[0])) * prime);
    }

    constexpr static inline uint32_t hash(char const*const aString, const size_t aStrlen, const uint32_t val)
    {
      return (aStrlen == 0) ? val : hash(aString + 1, aStrlen - 1, (val ^ uint32_t(aString[0])) * prime);
    }
  };

}

class string_hash
{
  public:
    constexpr string_hash(
      void
      ) = default;

    constexpr string_hash(
      const string_hash& other
      ) = default;

    constexpr string_hash(
      string_hash&& other
      ) = default;

    string_hash& operator=(
      const string_hash& other
      ) = default;

    string_hash& operator=(
      string_hash&& other
      ) = default;

    constexpr string_hash(
      const char* value
      )
      : _hash(detail::fnv1a<uint32_t>::hash(value))
    {

    }

    constexpr uint32_t
    get_hash() const
    {
      return _hash;
    }

    constexpr bool
    equals(
      const string_hash& other
      ) const
    {
      return _hash == other._hash;
    }

    friend constexpr bool operator==(
      const string_hash& lhs,
      const string_hash& rhs
      )
    {
      return lhs.equals(rhs);
    }

    friend constexpr bool operator!=(
      const string_hash& lhs,
      const string_hash& rhs
      )
    {
      return !(lhs == rhs);
    }

//     friend bool operator==(
//       const string_hash& lhs,
//       const string_ref rhs
//       )
//     {
//       return lhs._hash == detail::hash(rhs.get_buffer());
//     }
//
//     friend bool operator!=(
//       const string_hash& lhs,
//       const string_ref rhs
//       )
//     {
//       return !(lhs == rhs);
//     }
//
//     friend bool operator==(
//       const string_ref lhs,
//       const string_hash& rhs
//       )
//     {
//       return rhs._hash == detail::hash(lhs.get_buffer());
//     }
//
//     friend bool operator!=(
//       const string_ref lhs,
//       const string_hash& rhs
//       )
//     {
//       return !(lhs == rhs);
//     }

  private:
    uint32_t _hash = 0;
};

}
