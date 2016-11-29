#pragma once
#include <type_traits>

namespace mini {

template <typename T>
class flags final
{
  typedef void** empty;

  public:
    using enum_type = T;
    using underlying_type = typename std::underlying_type<T>::type;

    flags(empty = 0) : i(0) {}
    flags(T f) : i(static_cast<underlying_type>(f)) {}
    template <typename U>
    explicit flags(U f, typename std::enable_if<std::is_integral<U>::value, int>::type = 0) : i(f) {}

    flags(const flags& f) : i(f.i) {}

    flags& operator&=(flags f) { i &= f.i; return *this; }
    template <typename U>
    typename std::enable_if<std::is_integral<U>::value, flags&>::type operator&=(U mask) { i &= mask; return *this; }

    flags& operator|=(flags f) { i |= f.i; return *this; }
    flags& operator^=(flags f) { i ^= f.i; return *this; }

    operator underlying_type() const { return i; }

    flags operator~() const { return flags(static_cast<underlying_type>(~i)); }

    bool operator!() const { return !i; }

    bool test_flag(T f) const { return (i & static_cast<underlying_type>(f)) != 0; }

    friend flags operator^(flags lhs, flags rhs) { return flags(static_cast<underlying_type>(lhs.i ^ rhs.i)); }
    friend flags operator^(flags lhs, T f)       { return flags(static_cast<underlying_type>(lhs.i ^ static_cast<underlying_type>(f))); }
    friend flags operator^(T     f, flags lhs)   { return lhs ^ f; }

    friend flags operator|(flags lhs, flags rhs) { return flags(static_cast<underlying_type>(lhs.i | rhs.i)); }
    friend flags operator|(flags lhs, T f)       { return flags(static_cast<underlying_type>(lhs.i | static_cast<underlying_type>(f))); }
    friend flags operator|(T     f,   flags lhs) { return lhs | f; }

    friend flags operator&(flags lhs, flags rhs) { return flags(static_cast<underlying_type>(lhs.i & rhs.i)); }
    friend flags operator&(flags lhs, T rhs)     { return flags(static_cast<underlying_type>(lhs.i & static_cast<underlying_type>(rhs))); }
    friend flags operator&(T     lhs, flags rhs) { return lhs & rhs; }

    template <typename U>
    friend typename std::enable_if<std::is_integral<U>::value, flags&>::type operator&(flags flags, U f)
    { return flags(static_cast<underlying_type>(flags.i & f)); }

    template <typename U>
    friend typename std::enable_if<std::is_integral<U>::value, flags&>::type operator&(U f, flags flags)
    { return flags(static_cast<underlying_type>(flags.i & f)); }

    friend bool operator==(flags lhs, T rhs) { return lhs.i == static_cast<underlying_type>(rhs); }
    friend bool operator==(T lhs, flags rhs) { return rhs == lhs; }
    friend bool operator!=(flags lhs, T rhs) { return !(lhs == rhs); }
    friend bool operator!=(T lhs, flags rhs) { return !(lhs == rhs); }

  private:
    union
    {
      underlying_type i;
      T _i;
    };
};

#define DECLARE_FLAGS_OPERATORS(f)                                                \
  inline ::mini::flags<f::enum_type> operator|(f::enum_type f1, f::enum_type f2)  \
    { return ::mini::flags<f::enum_type>(f1) | f2; }                              \
  inline ::mini::flags<f::enum_type> operator~(f::enum_type f1)                   \
    { return ~::mini::flags<f::enum_type>(f1); }

}
