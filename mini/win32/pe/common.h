#pragma once
#include "../../common.h"

#include <cstdint>
#include <type_traits>

#pragma warning(disable: 4201)

#include <windows.h>

namespace mini::win32 {

enum class image_file_flags_t : uint16_t
{
  relocs_stripped           = 0x0001,  // Relocation info stripped from file.
  executable_image          = 0x0002,  // File is executable  (i.e. no unresolved external references).
  line_nums_stripped        = 0x0004,  // Line nunbers stripped from file.
  local_syms_stripped       = 0x0008,  // Local symbols stripped from file.
  aggresive_ws_trim         = 0x0010,  // Aggressively trim working set
  large_address_aware       = 0x0020,  // App can handle >2gb addresses
  bytes_reversed_lo         = 0x0080,  // Bytes of machine word are reversed.
  machine_32bit             = 0x0100,  // 32 bit word machine.
  debug_stripped            = 0x0200,  // Debugging info stripped from file in .DBG file
  removable_run_from_swap   = 0x0400,  // If Image is on removable media, copy and run from the swap file.
  net_run_from_swap         = 0x0800,  // If Image is on Net, copy and run from the swap file.
  system                    = 0x1000,  // System File.
  dll                       = 0x2000,  // File is a DLL.
  up_system_only            = 0x4000,  // File should only be run on a UP machine
  bytes_reversed_hi         = 0x8000,  // Bytes of machine word are reversed.
};

struct image_file_characteristics_t
{
  union
  {
    image_file_flags_t flags;

    struct
    {
      uint16_t relocs_stripped           : 1; // Relocation info stripped from file.
      uint16_t executable_image          : 1; // File is executable  (i.e. no unresolved external references).
      uint16_t line_nums_stripped        : 1; // Line nunbers stripped from file.
      uint16_t local_syms_stripped       : 1; // Local symbols stripped from file.
      uint16_t aggresive_ws_trim         : 1; // Aggressively trim working set
      uint16_t large_address_aware       : 1; // App can handle >2gb addresses
      uint16_t reserved                  : 1;
      uint16_t bytes_reversed_lo         : 1; // Bytes of machine word are reversed.
      uint16_t machine_32bit             : 1; // 32 bit word machine.
      uint16_t debug_stripped            : 1; // Debugging info stripped from file in .DBG file
      uint16_t removable_run_from_swap   : 1; // If Image is on removable media, copy and run from the swap file.
      uint16_t net_run_from_swap         : 1; // If Image is on Net, copy and run from the swap file.
      uint16_t system                    : 1; // System File.
      uint16_t dll                       : 1; // File is a DLL.
      uint16_t up_system_only            : 1; // File should only be run on a UP machine
      uint16_t bytes_reversed_hi         : 1; // Bytes of machine word are reversed.
    };
  };
};

enum class image_file_machine_t : uint16_t
{
  unknown           = 0     ,
  target_host       = 0x0001,  // Useful for indicating we want to interact with the host and not a WoW guest.
  i386              = 0x014c,  // Intel 386.
  r3000             = 0x0162,  // MIPS little-endian, 0x160 big-endian
  r4000             = 0x0166,  // MIPS little-endian
  r10000            = 0x0168,  // MIPS little-endian
  wcemipsv2         = 0x0169,  // MIPS little-endian WCE v2
  alpha             = 0x0184,  // Alpha_AXP
  sh3               = 0x01a2,  // SH3 little-endian
  sh3dsp            = 0x01a3,
  sh3e              = 0x01a4,  // SH3E little-endian
  sh4               = 0x01a6,  // SH4 little-endian
  sh5               = 0x01a8,  // SH5
  arm               = 0x01c0,  // ARM Little-Endian
  thumb             = 0x01c2,  // ARM Thumb/Thumb-2 Little-Endian
  armnt             = 0x01c4,  // ARM Thumb-2 Little-Endian
  am33              = 0x01d3,
  powerpc           = 0x01F0,  // IBM PowerPC Little-Endian
  powerpcfp         = 0x01f1,
  ia64              = 0x0200,  // Intel 64
  mips16            = 0x0266,  // MIPS
  alpha64           = 0x0284,  // ALPHA64
  mipsfpu           = 0x0366,  // MIPS
  mipsfpu16         = 0x0466,  // MIPS
  axp64             = alpha64,
  tricore           = 0x0520,  // Infineon
  cef               = 0x0CEF,
  ebc               = 0x0EBC,  // EFI Byte Code
  amd64             = 0x8664,  // AMD64 (K8)
  m32r              = 0x9041,  // M32R little-endian
  arm64             = 0xAA64,  // ARM64 Little-Endian
  cee               = 0xC0EE,
};

//
// Directory Entries.
//

enum class image_directory_t
{
  entry_export         =  0,   // Export Directory
  entry_import         =  1,   // Import Directory
  entry_resource       =  2,   // Resource Directory
  entry_exception      =  3,   // Exception Directory
  entry_security       =  4,   // Security Directory
  entry_basereloc      =  5,   // Base Relocation Table
  entry_debug          =  6,   // Debug Directory
  entry_architecture   =  7,   // Architecture Specific Data
  entry_globalptr      =  8,   // RVA of GP
  entry_tls            =  9,   // TLS Directory
  entry_load_config    = 10,   // Load Configuration Directory
  entry_bound_import   = 11,   // Bound Import Directory in headers
  entry_iat            = 12,   // Import Address Table
  entry_delay_import   = 13,   // Delay Load Import Descriptors
  entry_com_descriptor = 14,   // COM Runtime descriptor
};

#if defined(MINI_COMPILER_CLANG)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wpragma-pack"
#endif
#include <pshpack2.h>                         // 16 bit headers are 2 byte packed

struct image_dos_header_t
{                                             // DOS .EXE header
  uint16_t        e_magic;                    // Magic number
  uint16_t        e_cblp;                     // Bytes on last page of file
  uint16_t        e_cp;                       // Pages in file
  uint16_t        e_crlc;                     // Relocations
  uint16_t        e_cparhdr;                  // Size of header in paragraphs
  uint16_t        e_minalloc;                 // Minimum extra paragraphs needed
  uint16_t        e_maxalloc;                 // Maximum extra paragraphs needed
  uint16_t        e_ss;                       // Initial (relative) SS value
  uint16_t        e_sp;                       // Initial SP value
  uint16_t        e_csum;                     // Checksum
  uint16_t        e_ip;                       // Initial IP value
  uint16_t        e_cs;                       // Initial (relative) CS value
  uint16_t        e_lfarlc;                   // File address of relocation table
  uint16_t        e_ovno;                     // Overlay number
  uint16_t        e_res[4];                   // Reserved words
  uint16_t        e_oemid;                    // OEM identifier (for e_oeminfo)
  uint16_t        e_oeminfo;                  // OEM information; e_oemid specific
  uint16_t        e_res2[10];                 // Reserved words
  int32_t         e_lfanew;                   // File address of new exe header
};

#include <poppack.h>
#if defined(MINI_COMPILER_CLANG)
# pragma GCC diagnostic pop
#endif

//
// File header format.
//

struct image_file_header_t
{
  uint16_t        machine;
  uint16_t        number_of_sections;
  uint32_t        time_date_stamp;
  uint32_t        pointer_to_symbol_table;
  uint32_t        number_of_symbols;
  uint16_t        size_of_optional_header;
  uint16_t        characteristics;
};

//
// Directory format.
//

struct image_data_directory_t
{
  uint32_t        virtual_address;
  uint32_t        size;
};

constexpr auto image_number_of_directory_entries = 16;

//
// Optional header format.
//

struct image_optional_header_32_t
{
  //
  // Standard fields.
  //

  uint16_t        magic;
  uint8_t         major_linker_version;
  uint8_t         minor_linker_version;
  uint32_t        size_of_code;
  uint32_t        size_of_initialized_data;
  uint32_t        size_of_uninitialized_data;
  uint32_t        address_of_entry_point;
  uint32_t        base_of_code;
  uint32_t        base_of_data;

  //
  // NT additional fields.
  //

  uint32_t        image_base;
  uint32_t        section_alignment;
  uint32_t        file_alignment;
  uint16_t        major_operating_system_version;
  uint16_t        minor_operating_system_version;
  uint16_t        major_image_version;
  uint16_t        minor_image_version;
  uint16_t        major_subsystem_version;
  uint16_t        minor_subsystem_version;
  uint32_t        win32_version_value;
  uint32_t        size_of_image;
  uint32_t        size_of_headers;
  uint32_t        checksum;
  uint16_t        subsystem;
  uint16_t        dll_characteristics;
  uint32_t        size_of_stack_reserve;
  uint32_t        size_of_stack_commit;
  uint32_t        size_of_heap_reserve;
  uint32_t        size_of_heap_commit;
  uint32_t        loader_flags;
  uint32_t        number_of_rva_and_sizes;
  image_data_directory_t data_directory[image_number_of_directory_entries];
};

struct image_optional_header_64_t
{
  uint16_t        magic;
  uint8_t         major_linker_version;
  uint8_t         minor_linker_version;
  uint32_t        size_of_code;
  uint32_t        size_of_initialized_data;
  uint32_t        size_of_uninitialized_data;
  uint32_t        address_of_entry_point;
  uint32_t        base_of_code;
  uint64_t        image_base;
  uint32_t        section_alignment;
  uint32_t        file_alignment;
  uint16_t        major_operating_system_version;
  uint16_t        minor_operating_system_version;
  uint16_t        major_image_version;
  uint16_t        minor_image_version;
  uint16_t        major_subsystem_version;
  uint16_t        minor_subsystem_version;
  uint32_t        win32_version_value;
  uint32_t        size_of_image;
  uint32_t        size_of_headers;
  uint32_t        checksum;
  uint16_t        subsystem;
  uint16_t        dll_characteristics;
  uint64_t        size_of_stack_reserve;
  uint64_t        size_of_stack_commit;
  uint64_t        size_of_heap_reserve;
  uint64_t        size_of_heap_commit;
  uint32_t        loader_flags;
  uint32_t        number_of_rva_and_sizes;
  image_data_directory_t data_directory[image_number_of_directory_entries];
};

using image_optional_header_t = std::conditional_t<
  MINI_ARCH_BITS == 32,
  image_optional_header_32_t,
  image_optional_header_64_t
>;

//
// Section header format.
//

constexpr auto image_sizeof_short_name = 8;

struct image_section_header_t
{
  uint8_t         name[image_sizeof_short_name];
  union
  {
    uint32_t      physical_address;
    uint32_t      virtual_size;
  } misc;
  uint32_t        virtual_address;
  uint32_t        size_of_raw_data;
  uint32_t        pointer_to_raw_data;
  uint32_t        pointer_to_relocations;
  uint32_t        pointer_to_line_numbers;
  uint16_t        number_of_relocations;
  uint16_t        number_of_line_numbers;
  uint32_t        characteristics;
};

struct image_nt_headers_32_t
{
  uint32_t signature;
  image_file_header_t file_header;
  image_optional_header_32_t optional_header;
};

struct image_nt_headers_64_t
{
  uint32_t signature;
  image_file_header_t file_header;
  image_optional_header_64_t optional_header;
};

using image_nt_headers_t = std::conditional_t<
  MINI_ARCH_BITS == 32,
  image_nt_headers_32_t,
  image_nt_headers_64_t
>;

//
// DLL support.
//

//
// Export Format.
//

struct image_export_directory_t
{
  static constexpr auto index = image_directory_t::entry_export;

  uint32_t        characteristics;
  uint32_t        time_date_stamp;
  uint16_t        major_version;
  uint16_t        minor_version;
  uint32_t        name;
  uint32_t        base;
  uint32_t        number_of_functions;
  uint32_t        number_of_names;
  uint32_t        address_of_functions;     // RVA from base of image
  uint32_t        address_of_names;         // RVA from base of image
  uint32_t        address_of_name_ordinals; // RVA from base of image
};

//
// Import Format.
//

struct image_import_by_name_t
{
  uint16_t        hint;
  uint8_t         name[1];
};

struct image_thunk_data_32_t
{
  union
  {
    uint32_t      forwarder_string;       // PUCHAR
    uint32_t      function;               // PULONG
    uint32_t      ordinal;
    uint32_t      address_of_data;        // PIMAGE_IMPORT_BY_NAME
  };
};

#if defined(MINI_COMPILER_CLANG)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wpragma-pack"
#endif
#include <pshpack8.h>                     // Use align 8 for the 64-bit IAT.

struct image_thunk_data_64_t
{
  union
  {
    uint64_t      forwarder_string;       // PUCHAR
    uint64_t      function;               // PULONG
    uint64_t      ordinal;
    uint64_t      address_of_data;        // PIMAGE_IMPORT_BY_NAME
  };
};

#include <poppack.h>                      // Back to 4 byte packing
#if defined(MINI_COMPILER_CLANG)
# pragma GCC diagnostic pop
#endif

using image_thunk_data_t = std::conditional_t<
  MINI_ARCH_BITS == 32,
  image_thunk_data_32_t,
  image_thunk_data_64_t
>;

constexpr auto image_ordinal_flag_64 = 0x8000000000000000;
constexpr auto image_ordinal_flag_32 = 0x80000000;

struct image_import_descriptor_t
{
  static constexpr auto index = image_directory_t::entry_import;

  union
  {
    uint32_t      characteristics;        // 0 for terminating null import descriptor
    uint32_t      original_first_thunk;   // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
  };
  uint32_t        time_date_stamp;        // 0 if not bound,
                                          // -1 if bound, and real date\time stamp
                                          //     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
                                          // O.W. date/time stamp of DLL bound to (Old BIND)

  uint32_t        forwarder_chain;        // -1 if no forwarders
  uint32_t        name;
  uint32_t        first_thunk;            // RVA to IAT (if bound this IAT has actual addresses)
};

//
// Resource Format.
//

struct image_resource_directory_t
{
  static constexpr auto index = image_directory_t::entry_resource;

  uint32_t        characteristics;
  uint32_t        time_date_stamp;
  uint16_t        major_version;
  uint16_t        minor_version;
  uint16_t        number_of_named_entries;
  uint16_t        number_of_id_entries;
  //  image_resource_directory_entry_t directory_entries[];
};

constexpr auto image_resource_name_is_string    = 0x80000000;
constexpr auto image_resource_data_is_directory = 0x80000000;

struct image_resource_directory_entry_t
{
  union
  {
    struct
    {
      uint32_t    name_offset : 31;
      uint32_t    name_is_string : 1;
    };
    uint32_t      name;
    uint16_t      id;
  };
  union
  {
    uint32_t      offset_to_data;
    struct
    {
      uint32_t    offset_to_directory : 31;
      uint32_t    data_is_directory : 1;
    };
  };
};

struct image_resource_directory_string_t
{
  uint16_t        length;
  char            name_string[1];
};


struct image_resource_dir_string_u_t
{
  uint16_t        length;
  wchar_t         name_string[1];
};

struct image_resource_data_entry_t
{
  uint32_t        offset_to_data;
  uint32_t        size;
  uint32_t        code_page;
  uint32_t        reserved;
};

//
// Based relocation format.
//

struct image_base_relocation_t
{
  static constexpr auto index = image_directory_t::entry_basereloc;

  uint32_t        virtual_address;
  uint32_t        size_of_block;
//  uint16_t      type_offset[1];
};

//
// Thread Local Storage.
//

using image_tls_callback_fn_t = void (__stdcall *)(
    void* dll_handle,
    uint32_t reason,
    void* reserved
    );

struct image_tls_directory_32_t
{
  static constexpr auto index = image_directory_t::entry_tls;

  uint32_t        start_address_of_raw_data;
  uint32_t        end_address_of_raw_data;
  uint32_t        address_of_index;             // PDWORD
  uint32_t        address_of_callbacks;         // PIMAGE_TLS_CALLBACK *
  uint32_t        size_of_zero_fill;
  union
  {
    uint32_t      characteristics;
    struct
    {
      uint32_t    reserved0 : 20;
      uint32_t    alignment : 4;
      uint32_t    reserved1 : 8;
    };
  };
};

struct image_tls_directory_64_t
{
  static constexpr auto index = image_directory_t::entry_tls;

  uint64_t        start_address_of_raw_data;
  uint64_t        end_address_of_raw_data;
  uint64_t        address_of_index;         // PDWORD
  uint64_t        address_of_callbacks;     // PIMAGE_TLS_CALLBACK *;
  uint32_t        size_of_zero_fill;
  union
  {
    uint32_t      characteristics;
    struct
    {
      uint32_t    reserved0 : 20;
      uint32_t    alignment : 4;
      uint32_t    reserved1 : 8;
    };
  };
};
using image_tls_directory_t = std::conditional_t<
  MINI_ARCH_BITS == 32,
  image_tls_directory_32_t,
  image_tls_directory_64_t
>;

//////////////////////////////////////////////////////////////////////////

template <
  typename T = void
>
struct pe_directory
{
  T*     entry;
  size_t size;
};

struct image_32_t
{
  using image_dos_header_t                = image_dos_header_t;
  using image_file_header_t               = image_file_header_t;
  using image_data_directory_t            = image_data_directory_t;
  using image_optional_header_32_t        = image_optional_header_32_t;
  using image_section_header_t            = image_section_header_t;
  using image_nt_headers_t                = image_nt_headers_32_t;
  using image_export_directory_t          = image_export_directory_t;
  using image_import_by_name_t            = image_import_by_name_t;
  using image_thunk_data_t                = image_thunk_data_32_t;
  using image_import_descriptor_t         = image_import_descriptor_t;
  using image_resource_directory_t        = image_resource_directory_t;
  using image_resource_directory_entry_t  = image_resource_directory_entry_t;
  using image_resource_directory_string_t = image_resource_directory_string_t;
  using image_resource_dir_string_u_t     = image_resource_dir_string_u_t;
  using image_resource_data_entry_t       = image_resource_data_entry_t;
  using image_base_relocation_t           = image_base_relocation_t;
  using image_tls_directory_t             = image_tls_directory_32_t;

  static constexpr auto image_ordinal_flag = image_ordinal_flag_32;
};

struct image_64_t
{
  using image_dos_header_t                = image_dos_header_t;
  using image_file_header_t               = image_file_header_t;
  using image_data_directory_t            = image_data_directory_t;
  using image_optional_header_t           = image_optional_header_64_t;
  using image_section_header_t            = image_section_header_t;
  using image_nt_headers_t                = image_nt_headers_64_t;
  using image_export_directory_t          = image_export_directory_t;
  using image_import_by_name_t            = image_import_by_name_t;
  using image_thunk_data_t                = image_thunk_data_64_t;
  using image_import_descriptor_t         = image_import_descriptor_t;
  using image_resource_directory_t        = image_resource_directory_t;
  using image_resource_directory_entry_t  = image_resource_directory_entry_t;
  using image_resource_directory_string_t = image_resource_directory_string_t;
  using image_resource_dir_string_u_t     = image_resource_dir_string_u_t;
  using image_resource_data_entry_t       = image_resource_data_entry_t;
  using image_base_relocation_t           = image_base_relocation_t;
  using image_tls_directory_t             = image_tls_directory_64_t;

  static constexpr auto image_ordinal_flag = image_ordinal_flag_64;
};

using image_t = std::conditional_t<
  MINI_ARCH_BITS == 32,
  image_32_t,
  image_64_t
>;



}
