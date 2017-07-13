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
  typename ALLOCATOR,
  bool is_trivially_default_constructible
>
struct construct_range_impl
{
  template <
    typename ITERATOR_TYPE,
    typename ...ARGS
  >
  void operator() (
    ITERATOR_TYPE begin,
    ITERATOR_TYPE end,
    ARGS&&... args
    )
  {
    while (begin < end)
    {
      ALLOCATOR::construct(begin++, std::forward<ARGS>(args)...);
    }
  }
};

template <
  typename ALLOCATOR
>
struct construct_range_impl<ALLOCATOR, true>
{
  template <
    typename ITERATOR_TYPE,
    typename ...ARGS
  >
  void operator() (
    ITERATOR_TYPE begin,
    ITERATOR_TYPE end,
    ARGS&&... args
    )
  {
    MINI_UNREFERENCED_PARAMETER_PACK(args);

    memory::set(
      begin,
      typename ALLOCATOR::value_type(),
      (end - begin) * sizeof(typename ALLOCATOR::value_type));
  }
};

//
// move_range_impl.
//
template <
  typename ALLOCATOR,
  bool is_trivially_default_constructible
>
struct move_range_impl
{
  template <
    typename ITERATOR_TYPE_SRC,
    typename ITERATOR_TYPE_DEST
  >
  void operator() (
    ITERATOR_TYPE_SRC src_begin,
    ITERATOR_TYPE_SRC src_end,
    ITERATOR_TYPE_DEST dest_begin
    )
  {
    while (src_begin < src_end)
    {
      ALLOCATOR::construct(dest_begin++, std::move(*src_begin++));
    }
  }
};

template <
  typename ALLOCATOR
>
struct move_range_impl<ALLOCATOR, true>
{
  template <
    typename ITERATOR_TYPE_SRC,
    typename ITERATOR_TYPE_DEST
  >
  void operator() (
    ITERATOR_TYPE_SRC src_begin,
    ITERATOR_TYPE_SRC src_end,
    ITERATOR_TYPE_DEST dest_begin
    )
  {
    memory::move(dest_begin, src_begin, (src_end - src_begin) * sizeof(typename ALLOCATOR::value_type));
  }
};

//
// move_range_impl.
//
template <
  typename ALLOCATOR,
  bool is_trivially_destructible
>
struct destroy_range_impl
{
  template <
    typename ITERATOR_TYPE
  >
  void operator() (
    ITERATOR_TYPE begin,
    ITERATOR_TYPE end
    )
  {
    while (begin < end)
    {
      ALLOCATOR::destroy(begin++);
    }
  }
};

template <
  typename ALLOCATOR
>
struct destroy_range_impl<ALLOCATOR, true>
{
  template <
    typename ITERATOR_TYPE
  >
  void operator() (
    ITERATOR_TYPE begin,
    ITERATOR_TYPE end
    )
  {
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

    static T*
    allocate(
      size_type count
      )
    {
      size_type size = sizeof(T);
      return (T*)::operator new(size * count);
    }

    static void
    deallocate(
      T* pointer
      )
    {
      ::operator delete((void*)pointer);
    }

    static void
    deallocate(
      T* pointer,
      size_type count
      )
    {
      ::operator delete((void*)pointer);
    }

    template <
      typename ...ARGS
    >
    static void
    construct(
      T* pointer,
      ARGS&&... args
      )
    {
      new((void*)pointer) T(std::forward<ARGS>(args)...);
    }

    template <
      typename U
    >
    static void
    destroy(
      U* pointer
      )
    {
      MINI_UNREFERENCED(pointer); // MSVC bug?

      pointer->~U();
    }

    template <
      typename ITERATOR_TYPE,
      typename ...ARGS
    >
    static void
    construct_range(
      ITERATOR_TYPE begin,
      ITERATOR_TYPE end,
      ARGS&&... args
      )
    {
      detail::construct_range_impl<this_type, std::is_trivially_default_constructible_v<T>>()(
        begin,
        end,
        std::forward<ARGS>(args)...);
    }

    template <
      typename ITERATOR_TYPE_SRC,
      typename ITERATOR_TYPE_DEST
    >
    static void
    move_range(
      ITERATOR_TYPE_SRC src_begin,
      ITERATOR_TYPE_SRC src_end,
      ITERATOR_TYPE_DEST dest_begin
      )
    {
      detail::move_range_impl<this_type, std::is_trivially_default_constructible_v<T>>()(
        src_begin,
        src_end,
        dest_begin);
    }

    template <
      typename ITERATOR_TYPE
    >
    static void
    destroy_range(
      ITERATOR_TYPE begin,
      ITERATOR_TYPE end
      )
    {
      detail::destroy_range_impl<this_type, std::is_trivially_destructible_v<T>>()(
        begin,
        end);
    }
};

}
