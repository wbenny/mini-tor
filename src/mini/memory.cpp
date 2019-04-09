#include "memory.h"
#include "common.h"

#include <cstdlib>
#include <cstring>

namespace mini::memory {

namespace detail {

void*
memrchr(
  const void* haystack,
  int needle,
  size_t haystack_size
  )
{
  byte_type* haystack_bytes = const_cast<byte_type*>(reinterpret_cast<const byte_type*>(haystack));

  for (byte_type* cur = haystack_bytes + haystack_size - 1; cur != haystack_bytes - 1; cur--)
  {
    if (cur[0] == static_cast<byte_type>(needle))
    {
      return cur;
    }
  }

  return nullptr;
}

}

void*
allocate(
  size_t size
  )
{
  return ::malloc(size);
}

void*
reallocate(
  void* ptr,
  size_t new_size
  )
{
  return ::realloc(ptr, new_size);
}

void
free(
  void* ptr
  )
{
  ::free(ptr);
}

void*
copy(
  void* destination,
  const void* source,
  size_t size
  )
{
  return memcpy(destination, source, size);
}

void*
reverse(
  void* destination,
  size_t size
  )
{
  byte_type* destination_bytes = reinterpret_cast<byte_type*>(destination);

  for (size_t i = 0; i < (size / 2); i++)
  {
    swap(destination_bytes[i], destination_bytes[size - 1 - i]);
  }

  return destination;
}

void*
reverse_copy(
  void* destination,
  const void* source,
  size_t size
  )
{
  byte_type* destination_bytes = reinterpret_cast<byte_type*>(destination);
  const byte_type* source_bytes = reinterpret_cast<const byte_type*>(source);

  for (size_t i = 0; i < size; i++)
  {
    destination_bytes[size - 1 - i] = source_bytes[i];
  }

  return destination;
}

void*
move(
  void* destination,
  const void* source,
  size_t size
  )
{
  return memmove(destination, source, size);
}

int
compare(
  const void* lhs,
  const void* rhs,
  size_t size
  )
{
  return memcmp(lhs, rhs, size);
}

bool
equal(
  const void* rhs,
  const void* lhs,
  size_t size
  )
{
  return compare(lhs, rhs, size) == 0;
}

void*
find(
  const void* haystack,
  size_t haystack_size,
  const void* needle,
  size_t needle_size
  )
{
  byte_type* haystack_bytes = const_cast<byte_type*>(reinterpret_cast<const byte_type*>(haystack));
  byte_type* needle_bytes   = const_cast<byte_type*>(reinterpret_cast<const byte_type*>(needle));

  if (haystack_size == 0 || needle_size == 0)
  {
    return nullptr;
  }

  if (haystack_size < needle_size)
  {
    return nullptr;
  }

  //
  // special case when needle size == 1.
  //
  if (needle_size == 1)
  {
    return (void*)memchr(haystack, (int)*needle_bytes, haystack_size);
  }

  byte_type* last = haystack_bytes + haystack_size - needle_size;
  for (byte_type* cur = haystack_bytes; cur <= last; cur++)
  {
    if (cur[0] == needle_bytes[0] && memcmp(cur, needle_bytes, needle_size) == 0)
    {
      return cur;
    }
  }

  return nullptr;
}

void*
reverse_find(
  const void* haystack,
  size_t haystack_size,
  const void* needle,
  size_t needle_size
  )
{
  byte_type* haystack_bytes = const_cast<byte_type*>(reinterpret_cast<const byte_type*>(haystack));
  byte_type* needle_bytes   = const_cast<byte_type*>(reinterpret_cast<const byte_type*>(needle));

  if (haystack_size == 0 || needle_size == 0)
  {
    return nullptr;
  }

  if (haystack_size < needle_size)
  {
    return nullptr;
  }

  //
  // special case when needle size == 1.
  //
  if (needle_size == 1)
  {
    return (void*)detail::memrchr(haystack, (int)*needle_bytes, haystack_size);
  }

  for (byte_type* cur = haystack_bytes + haystack_size - 1; cur != haystack_bytes - 1; cur--)
  {
    if (cur[0] == needle_bytes[0] && memcmp(cur, needle_bytes, needle_size) == 0)
    {
      return cur;
    }
  }

  return nullptr;
}

void*
set(
  void* destination,
  int value,
  size_t size
  )
{
  return memset(destination, value, size);
}

void*
zero(
  void* destination,
  size_t size
  )
{
  return memset(destination, 0, size);
}

}
