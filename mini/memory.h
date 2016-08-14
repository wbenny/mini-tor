#pragma once
#include <cstdlib>
#include <cstring>
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
deallocate(
  void* ptr
  );

void*
copy(
  void* destination,
  const void* source,
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
zero(
  void* destination,
  size_t size
  );

template <
  typename T,
  typename = std::enable_if_t<std::is_pod_v<T>>
>
T&
zero(
  T& destination
  )
{
  return *reinterpret_cast<T*>(zero(&destination, sizeof(destination)));
}

template <
  typename T,
  size_t N,
  typename = std::enable_if_t<std::is_pod_v<T>>
>
T*
zero(
  T (&destination)[N]
  )
{
  return reinterpret_cast<T*>(zero(destination, sizeof(T) * N));
}

}
