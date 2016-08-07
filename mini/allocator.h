#pragma once
#include <mini/common.h>

#include <new>
#include <utility>
#include <type_traits>

namespace mini {

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
    using value_type              = T;

    using pointer                 = value_type*;
    using const_pointer           = const value_type*;

    static T*
    allocate(
      size_t count
      )
    {
      size_t size = sizeof(T);
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
      size_t count
      )
    {
      ::operator delete((void*)pointer);
    }

    template <
      typename... ARGS
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
      pointer->~U();
    }

    template <
      typename ITERATOR_TYPE,
      typename... ARGS
    >
    static void
    construct_range(
      ITERATOR_TYPE begin,
      ITERATOR_TYPE end,
      ARGS&&... args
      )
    {
      construct_range_impl<std::is_trivially_default_constructible_v<T>>()(
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
      move_range_impl<std::is_trivially_default_constructible_v<T>>()(
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
      destroy_range_impl<std::is_trivially_destructible_v<T>>()(
        begin,
        end);
    }

  private:
    //
    // construct_range_impl.
    //
    template <
      bool is_trivially_default_constructible
    >
    struct construct_range_impl
    {
      template <
        typename ITERATOR_TYPE,
        typename... ARGS
      >
      void operator() (
        ITERATOR_TYPE begin,
        ITERATOR_TYPE end,
        ARGS&&... args
        )
      {
        while (begin < end)
        {
          construct(begin++, std::forward<ARGS>(args)...);
        }
      }
    };

    template <>
    struct construct_range_impl<true>
    {
      template <
        typename ITERATOR_TYPE,
        typename... ARGS
      >
      void operator() (
        ITERATOR_TYPE begin,
        ITERATOR_TYPE end,
        ARGS&&... args
        )
      {

      }
    };

    //
    // move_range_impl.
    //
    template <
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
          construct(dest_begin++, std::move(*src_begin++));
        }
      }
    };

    template <>
    struct move_range_impl<true>
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
        memory::move(dest_begin, src_begin, (src_end - src_begin) * sizeof(*src_begin));
      }
    };

    //
    // move_range_impl.
    //
    template <
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
          destroy(begin++);
        }
      }
    };

    template <>
    struct destroy_range_impl<true>
    {
      template <
        typename ITERATOR_TYPE
      >
      void operator() (
        ITERATOR_TYPE begin,
        ITERATOR_TYPE end
        )
      {

      }
    };
};

}
