#pragma once
#include <mini/common.h>

#include <new>
#include <utility>

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

    T*
    allocate(
      size_t count
      )
    {
      size_t size = sizeof(T);
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
      size_t count
      )
    {
      ::operator delete((void*)pointer);
    }

    template <
      typename... ARGS
    >
    void
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
    void
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
    void
    construct_range(
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

    template <
      typename ITERATOR_TYPE
    >
    void
    destroy_range(
      ITERATOR_TYPE begin,
      ITERATOR_TYPE end
      )
    {
      while (begin < end)
      {
        destroy(begin++);
      }
    }

    template <
      typename ITERATOR_TYPE_SRC,
      typename ITERATOR_TYPE_DEST
    >
    void
    move_range(
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

}
