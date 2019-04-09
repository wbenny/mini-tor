#pragma once
#include <mini/common.h>
#include <mini/memory.h>

#include <new>
#include <utility>
#include <type_traits>

namespace mini {

namespace detail {

//
// construct_range_impl.
//
template <
  typename Allocator,
  bool is_trivially_default_constructible
>
struct construct_range
{
  template <
    typename TIterator,
    typename ...Args
  >
  void
  operator() (
    Allocator& allocator,
    TIterator begin,
    TIterator end,
    Args&&... args
    )
  {
    while (begin < end)
    {
      allocator.construct(begin++, std::forward<Args>(args)...);
    }
  }
};

template <
  typename Allocator
>
struct construct_range<Allocator, true>
{
  template <
    typename TIterator,
    typename ...Args
  >
  void
  operator() (
    Allocator& allocator,
    TIterator begin,
    TIterator end,
    Args&&... args
    )
  {
    MINI_UNREFERENCED(allocator);

    memory::set(begin, args..., (end - begin) *
                                sizeof(typename Allocator::value_type));
  }
};

//
// move_range_impl.
//
template <
  typename Allocator,
  bool is_trivially_default_constructible
>
struct move_range
{
  template <
    typename TIteratorSource,
    typename TIteratorDestination
  >
  void
  operator() (
    Allocator& allocator,
    TIteratorSource src_begin,
    TIteratorSource src_end,
    TIteratorDestination dest_begin
    )
  {
    while (src_begin < src_end)
    {
      allocator.construct(dest_begin++, std::move(*src_begin++));
    }
  }
};

template <
  typename Allocator
>
struct move_range<Allocator, true>
{
  template <
    typename TIteratorSource,
    typename TIteratorDestination
  >
  void
  operator() (
    Allocator& allocator,
    TIteratorSource src_begin,
    TIteratorSource src_end,
    TIteratorDestination dest_begin
    )
  {
    MINI_UNREFERENCED(allocator);

    memory::move(dest_begin, src_begin, (src_end - src_begin) * sizeof(typename Allocator::value_type));
  }
};

//
// move_range_impl.
//
template <
  typename Allocator,
  bool is_trivially_destructible
>
struct destroy_range
{
  template <
    typename TIterator
  >
  void
  operator() (
    Allocator& allocator,
    TIterator begin,
    TIterator end
    )
  {
    while (begin < end)
    {
      allocator.destroy(begin++);
    }
  }
};

template <
  typename Allocator
>
struct destroy_range<Allocator, true>
{
  template <
    typename TIterator
  >
  void
  operator() (
    Allocator& allocator,
    TIterator begin,
    TIterator end
    )
  {
    MINI_UNREFERENCED(allocator);
    MINI_UNREFERENCED(begin);
    MINI_UNREFERENCED(end);
  }
};

}

//
// allocator_arg_t alternative.
//
struct allocator_argument_type { };

template <
  typename T
>
class allocator
{
  public:
    using this_type               = allocator<T>;
    using value_type              = T;

    using pointer                 = value_type*;
    using const_pointer           = const value_type*;

    template <
      typename TOther
    >
    struct rebind
    {
      using other = allocator<TOther>;
    };

    T*
    allocate(
      size_type count
      )
    {
      size_type size = sizeof(T);
      return (T*)::operator new(size * count);
    }

    void
    deallocate(
      T* pointer
      )
    {
      ::operator delete((void*)pointer);
    }

    void
    deallocate(
      T* pointer,
      size_type count
      )
    {
      ::operator delete((void*)pointer);
    }

    template <
      typename ...Args
    >
    void
    construct(
      T* pointer,
      Args&&... args
      )
    {
      new((void*)pointer) T(std::forward<Args>(args)...);
    }

    template <
      typename U
    >
    void
    destroy(
      U* pointer
      )
    {
      MINI_UNREFERENCED(pointer); // MSVC bug?

      pointer->~U();
    }

    template <
      typename TIterator,
      typename ...Args
    >
    void
    construct_range(
      TIterator begin,
      TIterator end,
      Args&&... args
      )
    {
      detail::construct_range<
        this_type,
        std::is_trivially_default_constructible_v<T>
      >()(*this, begin, end, std::forward<Args>(args)...);
    }

    template <
      typename TIteratorSource,
      typename TIteratorDestination
    >
    void
    move_range(
      TIteratorSource src_begin,
      TIteratorSource src_end,
      TIteratorDestination dest_begin
      )
    {
      detail::move_range<
        this_type,
        std::is_trivially_default_constructible_v<T>
      >()(*this, src_begin, src_end, dest_begin);
    }

    template <
      typename TIterator
    >
    void
    destroy_range(
      TIterator begin,
      TIterator end
      )
    {
      detail::destroy_range<
        this_type,
        std::is_trivially_destructible_v<T>
      >()(*this, begin, end);
    }
};

}
