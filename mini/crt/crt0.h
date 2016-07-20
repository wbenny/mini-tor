#ifndef _DEBUG
#include <cstddef>

#define MINI_MSVCRT_LIB

extern "C" {

#pragma region CRT startup

extern int __argc;
extern char** __argv;

extern int __cdecl
main(
  int argc,
  char* argv[]
  );

void __cdecl
mainCRTStartup(
    void
  );

#pragma endregion

#pragma region CRT internal functions

//
// The default pure virtual function call error handler.
// The compiler generates code to call this function when
// a pure virtual member function is called.
//
int __cdecl
_purecall(void);

#pragma endregion

#pragma region CRT standard functions

#pragma region stdio.h

#pragma endregion

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

void* __cdecl
malloc(
  size_t size
  );

void* __cdecl
calloc(
  size_t count,
  size_t size
  );

void* __cdecl
realloc(
  void* ptr,
  size_t new_size
  );

void __cdecl
free(
  void* ptr
  );

#pragma endregion

#pragma region Program

typedef void(__cdecl *atexit_fn_t)(void);

[[noreturn]]
void __cdecl
abort(
  void
  );

[[noreturn]]
void __cdecl
exit(
  int exit_code
  );

int __cdecl
atexit(
  atexit_fn_t func
  );

#pragma endregion

#pragma endregion

#pragma region string.h

#pragma region Byte

int __cdecl
memcmp(
  const void* lhs,
  const void* rhs,
  size_t count
  );

void* __cdecl
memset(
  void* dest,
  int ch,
  size_t count
  );

void* __cdecl
memcpy(
  void* dest,
  const void* src,
  size_t count
  );

void* __cdecl
memmove(
  void *dest,
  const void *src,
  size_t count
  );

#pragma endregion

#pragma region String

int __cdecl
strncmp(
  const char* lhs,
  const char* rhs,
  size_t count
  );

const char* __cdecl
strstr(
  const char *str,
  const char *substr
  );

#pragma endregion

#pragma endregion

#pragma endregion

}

extern "C++" {

#pragma region new/delete

void* __cdecl
operator new(
  size_t size
  );

void* __cdecl
operator new[](
  size_t size
  );

void __cdecl
operator delete(
  void* pointer
  );

void __cdecl
operator delete(
  void* pointer,
  size_t
  );

void __cdecl
operator delete[](
  void* pointer
  );

#pragma endregion

}

#pragma endregion

#endif
