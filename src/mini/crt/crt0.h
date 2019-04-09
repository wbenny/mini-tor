#pragma once
#if __has_include(<mini/common.h>)
# include <mini/common.h>
#else
# define MINI_CONFIG_NO_DEFAULT_LIBS
# define MINI_MODE_KERNEL
# define MINI_CRT0_IGNORED (MINI_CRT0_STRING_BYTE | MINI_CRT0_NEW_DELETE)
#endif

#define MINI_CRT0_STARTUP         0x01  // mainCRTStartup (create_argc_and_argv, destroy_argc_and_argv)
#define MINI_CRT0_STDLIB_MEMORY   0x02  // malloc, calloc, realloc, free
#define MINI_CRT0_STDLIB_PROGRAM  0x04  // abort, exit, atexit
#define MINI_CRT0_STRING_BYTE     0x08  // memcmp, memset, memcpy, memmove
#define MINI_CRT0_STRING_STRING   0x10  // strncmp, strstr
#define MINI_CRT0_NEW_DELETE      0x20  // operator new/operator delete

//
// Some functions are imported from the
// msvcrt.dll. Therefore, ignore their
// implementation here.
//

#if !defined(MINI_CRT0_IGNORED)
# define MINI_CRT0_IGNORED         ( \
    MINI_CRT0_STDLIB_MEMORY   |       \
    MINI_CRT0_STDLIB_PROGRAM  |       \
    MINI_CRT0_STRING_BYTE     |       \
    MINI_CRT0_STRING_STRING           \
  )
#endif // !defined(MINI_CRT0_IGNORED)

//
// Provide alternative CRT only for
// compilations with /NODEFAULTLIB.
//

#if !defined(MINI_CONFIG_NO_DEFAULT_LIBS)
# include <cstdio>
# include <cstdlib>
# include <cstddef>
#else // !defined(MINI_CONFIG_NO_DEFAULT_LIBS)
# include <cstddef>

extern "C" {

#pragma region CRT startup

void
__cdecl
crt0_initialize(
  void
  );

void
__cdecl
crt0_destroy(
  void
  );

namespace crt0
{
  inline void initialize() { ::crt0_initialize(); }
  inline void destroy()    { ::crt0_destroy();    }
}

#if !defined(MINI_MODE_KERNEL)

extern int __argc;
extern char** __argv;

extern int
__cdecl
main(
  int argc,
  char* argv[]
  );

void
__cdecl
mainCRTStartup(
  void
  );

#endif // !defined(MINI_MODE_KERNEL)

#pragma endregion CRT startup

#pragma region CRT internal functions

//
// The default pure virtual function call error handler.
// The compiler generates code to call this function when
// a pure virtual member function is called.
//
int
__cdecl
_purecall(
  void
  );

#pragma endregion CRT internal functions

#pragma region CRT standard functions

#pragma region stdlib.h

#pragma region Memory

//
// Function prototypes.
//

typedef void* (__cdecl *malloc_fn_t)(
  size_t size
  );

typedef void* (__cdecl *calloc_fn_t)(
  size_t count,
  size_t size
  );

typedef void* (__cdecl *realloc_fn_t)(
  void* ptr,
  size_t new_size
  );

typedef void  (__cdecl *free_fn_t)(
  void* ptr
  );

__declspec(restrict)
void*
__cdecl
malloc(
  size_t size
  );

__declspec(restrict)
void*
__cdecl
calloc(
  size_t count,
  size_t size
  );

__declspec(restrict)
void*
__cdecl
realloc(
  void* ptr,
  size_t new_size
  );

void
__cdecl
free(
  void* ptr
  );

#pragma endregion Memory

#pragma region Program

typedef void (__cdecl *atexit_fn_t)(void);

[[noreturn]]
void
__cdecl
abort(
  void
  );

[[noreturn]]
void
__cdecl
exit(
  int exit_code
  );

int
__cdecl
atexit(
  atexit_fn_t func
  );

#pragma endregion Program

#pragma endregion stdlib.h

#pragma region string.h

#pragma region Byte

int
__cdecl
memcmp(
  const void* lhs,
  const void* rhs,
  size_t count
  );

void*
__cdecl
memset(
  void* dest,
  int ch,
  size_t count
  );

void*
__cdecl
memcpy(
  void* dest,
  const void* src,
  size_t count
  );

void*
__cdecl
memmove(
  void *dest,
  const void *src,
  size_t count
  );

#pragma endregion Byte

#pragma region String

int
__cdecl
strncmp(
  const char* lhs,
  const char* rhs,
  size_t count
  );

const char*
__cdecl
strstr(
  const char *str,
  const char *substr
  );

#pragma endregion String

#pragma endregion string.h

#pragma endregion CRT standard functions

}

extern "C++" {

#pragma region new/delete

void*
__cdecl
operator new(
  size_t size
  );

void*
__cdecl
operator new[](
  size_t size
  );

void
__cdecl
operator delete(
  void* pointer
  ) noexcept;

void
__cdecl
operator delete(
  void* pointer,
  size_t
  ) noexcept;

void
__cdecl
operator delete[](
  void* pointer
  ) noexcept;

void
__cdecl
operator delete[](
  void* pointer,
  size_t
  ) noexcept;

#pragma endregion new/delete

}

#endif // !defined(MINI_CONFIG_NO_DEFAULT_LIBS)
