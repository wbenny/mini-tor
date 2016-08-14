#include <windows.h>
#include <winternl.h>

#define EH_MAGIC_NUMBER1        0x19930520    
#define EH_PURE_MAGIC_NUMBER1   0x01994000
#define EH_EXCEPTION_NUMBER     ('msc' | 0xE0000000)

#if defined(_AMD64_)
#define EH_EXCEPTION_PARAMETERS 4
#else
#define EH_EXCEPTION_PARAMETERS 3
#endif

extern "C"
NTSYSAPI
VOID
NTAPI
RtlRaiseException(
  _In_ PEXCEPTION_RECORD ExceptionRecord
  );

namespace mini {

//
// despite the static typing, function pointers of 
//   type _PMFN are usually used as pointers to other
//   function types
//

typedef void (__cdecl * _PMFN)(void); 

struct _PMD
{
  int mdisp;  // member offset
  int pdisp;  // offset of the vtable 
  int vdisp;  // offset to displacment inside the vtable
};

struct CatchableType
{
  unsigned int     properties;            // bit 1: is a simple type
                                          //   bit 2: can be caught by reference only
                                          //   bit 3: has virtual base 
  void*            pType;                 // pointer to std::type_info object.
  _PMD             thisDisplacement;      // displacement of the object for this type
  int              sizeOrOffset;          // size of the type
  _PMFN            copyFunction;          // pointer to copy constructor or 0
};

struct CatchableTypeArray
{
  int              nCatchableTypes;       // number of entries in arrayOfCatchableTypes
  CatchableType**  arrayOfCatchableTypes; // array of pointers to all base types of the object
                                          //   including types that it can be implicitly cast to
};

struct ThrowInfo
{
  unsigned int     attributes;            // bit 1 for const, bit 2 for volatile
  _PMFN            pmfnUnwind;            // function to destroy the exception object
  int (__cdecl * pForwardCompat)(...);
  CatchableTypeArray* pCatchableTypeArray;// pointer to array of pointers to type information
};

}

extern "C"
[[noreturn]]
void __stdcall
mini_ThrowException(
  void*             pExceptionObject,     // The object thrown
  mini::ThrowInfo*  pThrowInfo            // Everything we need to know about it
  )
{
  //
  // A generic exception record.
  //
#if defined(_AMD64_)
  PVOID ThrowImageBase;
#endif

  EXCEPTION_RECORD ThisException = {
    EH_EXCEPTION_NUMBER,                  // Exception number
    EXCEPTION_NONCONTINUABLE,             // Exception flags (we don't do resume)
    nullptr,                              // Additional record (none)
    nullptr,                              // Address of exception (OS fills in)
    EH_EXCEPTION_PARAMETERS,              // Number of parameters
    {
      EH_MAGIC_NUMBER1,                   // Our version control magic number
      UINT_PTR(pExceptionObject),         // pExceptionObject
      UINT_PTR(pThrowInfo),               // pThrowInfo
#if defined(_AMD64_)
      UINT_PTR(RtlPcToFileHeader(pThrowInfo, &ThrowImageBase)),
#endif;
    }
  };

  //
  // Hand it off to the OS.
  //
  RtlRaiseException(&ThisException);
}

#include "crt0.h"
extern "C"
[[noreturn]]
void __cdecl
__std_terminate(
  void
  )
{
  exit(3);
}
