#pragma once

namespace mini {

  template <
    typename T = void
  >
  struct equal_to
  {
    constexpr bool
    operator()(
      const T& lhs,
      const T& rhs
      ) const
    {
      return (lhs == rhs);
    }
  };

  template <
    typename T = void
  >
  struct less
  {
    constexpr bool
    operator()(
      const T& lhs,
      const T& rhs
      ) const
    {
      return (lhs < rhs);
    }
  };

}
