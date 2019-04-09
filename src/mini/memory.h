#pragma once
#include <type_traits>

namespace mini::memory {

void*
allocate(
  size_t size
  );

void*
reallocate(
  void* ptr,
  size_t new_size
  );

void
free(
  void* ptr
  );

void*
copy(
  void* destination,
  const void* source,
  size_t size
  );

void*
reverse(
  void* destination,
  size_t size
  );

void*
reverse_copy(
  void* destination,
  const void* source,
  size_t size
  );

void*
move(
  void* destination,
  const void* source,
  size_t size
  );

int
compare(
  const void* lhs,
  const void* rhs,
  size_t size
  );

bool
equal(
  const void* rhs,
  const void* lhs,
  size_t size
  );

void*
find(
  const void *haystack,
  size_t haystack_size,
  const void *needle,
  size_t needle_size
  );

void*
reverse_find(
  const void *haystack,
  size_t haystack_size,
  const void *needle,
  size_t needle_size
  );

void*
set(
  void* destination,
  int value,
  size_t size
  );

void*
zero(
  void* destination,
  size_t size
  );

template <
  typename T
>
T&
zero(
  T& destination
  )
{
  static_assert(std::is_pod_v<T>);
  return *reinterpret_cast<T*>(zero(&destination, sizeof(destination)));
}

template <
  typename T,
  size_t N
>
T*
zero(
  T (&destination)[N]
  )
{
  static_assert(std::is_pod_v<T>);
  return reinterpret_cast<T*>(zero(destination, sizeof(T) * N));
}

}
