#ifndef _DEBUG
#include "crt0.h"

#include <cstdlib>

#include <windows.h>
#include <shellapi.h>

extern "C" {

#pragma region CRT initialization

// +++++++++++++++++++++++++++++++++++++++++++
//
// Prototypes.
//
// -------------------------------------------

//
// Put .CRT data into .rdata section
//
#pragma comment(linker, "/merge:.CRT=.rdata")

//
// C initializer function prototype.
//
typedef int (__cdecl *_PIFV)(void);

//
// Linker puts constructors between these sections, and we use them to locate constructor pointers.
//
#pragma section(".CRT$XIA", long, read)
#pragma section(".CRT$XIZ", long, read)

//
// Pointers surrounding constructors.
//
__declspec(allocate(".CRT$XIA")) _PIFV __xi_a[] = { 0 };
__declspec(allocate(".CRT$XIZ")) _PIFV __xi_z[] = { 0 };
extern __declspec(allocate(".CRT$XIA")) _PIFV __xi_a[];
extern __declspec(allocate(".CRT$XIZ")) _PIFV __xi_z[];

static int __cdecl
_initterm_e(
  _PIFV* const first,
  _PIFV* const last
  );

//
// C++ initializer function prototype.
//
typedef void (__cdecl *_PVFV)(void);

//
// Linker puts constructors between these sections, and we use them to locate constructor pointers.
//
#pragma section(".CRT$XCA", long, read)
#pragma section(".CRT$XCZ", long, read)

//
// Pointers surrounding constructors.
//
__declspec(allocate(".CRT$XCA")) _PVFV __xc_a[] = { 0 };
__declspec(allocate(".CRT$XCZ")) _PVFV __xc_z[] = { 0 };
extern __declspec(allocate(".CRT$XCA")) _PVFV __xc_a[];
extern __declspec(allocate(".CRT$XCZ")) _PVFV __xc_z[];

static void __cdecl
_initterm(
  _PVFV* const first,
  _PVFV* const last
  );

// +++++++++++++++++++++++++++++++++++++++++++
//
// Implemetation.
//
// -------------------------------------------

int __cdecl
_initterm_e(
  _PIFV* const first,
  _PIFV* const last
  )
{
  //
  // C initialization.
  //
  for (_PIFV* it = first; it != last; ++it)
  {
    if (*it == nullptr)
    {
      continue;
    }

    const int result = (**it)();
    if (result != 0)
    {
      return result;
    }
  }

  return 0;
}

void __cdecl
_initterm(
  _PVFV* const first,
  _PVFV* const last
  )
{
  //
  // C++ initialization.
  //
  for (_PVFV* it = first; it != last; ++it)
  {
    if (*it == nullptr)
      continue;

    (**it)();
  }
}

#pragma endregion

#pragma region CRT startup

int _fltused = 0;

int __argc;
char** __argv;

void
create_argc_and_argv(
  void
  )
{
  wchar_t** argvW = CommandLineToArgvW(GetCommandLineW(), &__argc);

  __argv = (char**)malloc(__argc * sizeof(char*));
  for (int i = 0; i < __argc; i++)
  {
    size_t argv_size = wcslen(argvW[i]) + 1;

    __argv[i] = (char*)malloc(argv_size);
    wcstombs(__argv[i], argvW[i], argv_size);
  }

  LocalFree(argvW);
}

void
destroy_argc_and_argv(
  void
  )
{
  for (int i = 0; i < __argc; i++)
  {
    free(__argv[i]);
  }

  free(__argv);
}

void __cdecl
mainCRTStartup(
  void
  )
{
  //
  // Call C initializers.
  //
  _initterm_e(__xi_a, __xi_z);

  //
  // Call C++ initializers.
  //
  _initterm(__xc_a, __xc_z);

  //
  // Exit with whatever main will return.
  //
  create_argc_and_argv();
  int exit_code = main(__argc, __argv);
  destroy_argc_and_argv();

  exit(exit_code);
}

#pragma endregion

#pragma region CRT internal functions

int __cdecl
_purecall(
  void
  )
{
  return 0;
}

#pragma endregion

#pragma region CRT standard functions

#ifndef MINI_MSVCRT_LIB

#pragma region stdio.h

#pragma endregion

#pragma region stdlib.h

#pragma region Memory

void* __cdecl
malloc(
  size_t size
  )
{
  //
  // UNIMPLEMENTED
  //
  return nullptr;
}

void* __cdecl
calloc(
  size_t count,
  size_t size
  )
{
  //
  // UNIMPLEMENTED
  //
  return nullptr;
}

void* __cdecl
realloc(
  void* ptr,
  size_t new_size
  )
{
  //
  // UNIMPLEMENTED
  //
  return nullptr;
}

void __cdecl
free(
  void* ptr
  )
{
  //
  // UNIMPLEMENTED
  //
  return;
}

#pragma endregion

#pragma region Program

static const size_t g_atexit_fn_max_count = 32;
static atexit_fn_t  g_atexit_fn_table[g_atexit_fn_max_count];
static size_t       g_atexit_fn_count = 0;

[[noreturn]]
void __cdecl
abort(
  void
  )
{
  exit(-1);
}

[[noreturn]]
void __cdecl
exit(
  int exit_code
  )
{
  while (g_atexit_fn_count-- > 0)
  {
    g_atexit_fn_table[g_atexit_fn_count]();
  }

  ExitProcess(exit_code);
}

int __cdecl
atexit(
  atexit_fn_t func
  )
{
  if (g_atexit_fn_count == g_atexit_fn_max_count)
  {
    return 1;
  }

  g_atexit_fn_table[g_atexit_fn_count++] = func;
  return 0;
}

#pragma endregion

#pragma endregion

#pragma region string.h

#pragma region Byte

#pragma function(memcmp)
int __cdecl
memcmp(
  const void* lhs,
  const void* rhs,
  size_t count
  )
{
  unsigned char u1, u2;
  unsigned char* s1 = (unsigned char*)lhs;
  unsigned char* s2 = (unsigned char*)rhs;

  for (; count--; s1++, s2++)
  {
    u1 = *(unsigned char *)s1;
    u2 = *(unsigned char *)s2;

    if (u1 != u2)
    {
      return (u1 - u2);
    }
  }

  return 0;
}

#pragma function(memset)
void* __cdecl
memset(
  void* dest,
  int ch,
  size_t count
  )
{
  if (count)
  {
    char* d = (char*)dest;

    do
    {
      *d++ = ch;
    } while (--count);
  }

  return dest;
}

#pragma function(memcpy)
void* __cdecl
memcpy(
  void* dest,
  const void* src,
  size_t count
  )
{
  char* d = (char*)dest;
  char* s = (char*)src;

  while (count--)
  {
    *d++ = *s++;
  }

  return dest;
}

void* __cdecl
memmove(
  void *dest,
  const void *src,
  size_t count
  )
{
  const char *s = (const char*)src;
  char *d = (char*)dest;

  if (!count)
  {
    return dest;
  }

  if (dest <= s)
  {
    return memcpy(dest, s, count);
  }

  s += count;
  d += count;

  while (count--)
  {
    *--d = *--s;
  }

  return dest;
}

#pragma endregion

#pragma region String

int __cdecl
strncmp(
  const char* lhs,
  const char* rhs,
  size_t count
  )
{
  for (; count > 0; lhs++, rhs++, --count)
  {
    if (*lhs != *rhs)
    {
      return ((*(unsigned char*)lhs < *(unsigned char*)rhs) ? -1 : +1);
    }
    else if (*lhs == '\0')
    {
      return 0;
    }
  }

  return 0;
}

const char* __cdecl
strstr(
  const char *str,
  const char *substr
  )
{
  char c;
  size_t len;

  c = *substr++;

  if (!c)
  {
    return (char*)str;
  }

  len = strlen(substr);
  do
  {
    char sc;

    do
    {
      sc = *str++;

      if (!sc)
      {
        return nullptr;
      }
    } while (sc != c);
  } while (strncmp(str, substr, len) != 0);

  return (char*)(str - 1);
}

#pragma endregion

#pragma endregion

#endif

#pragma endregion

}

extern "C++" {

#pragma region new/delete

void* __cdecl
operator new(
  size_t size
  )
{
  return malloc(size);
}

void* __cdecl
operator new[](
  size_t size
  )
{
  return malloc(size);
}

void __cdecl
operator delete(
  void* pointer
  )
{
  free(pointer);
}

void __cdecl
operator delete(
  void* pointer,
  size_t
  )
{
  free(pointer);
}

void __cdecl
operator delete[](
  void* pointer
  )
{
  free(pointer);
}

#pragma endregion

}

#endif
