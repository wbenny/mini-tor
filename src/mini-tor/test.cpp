#ifdef MINI_TEST

#define _CRTDBG_MAP_ALLOC
#include <mini/memory.h>
#include <stdlib.h>
#include <crtdbg.h>

#include <mini/collections/list.h>
#include <mini/collections/pair_list.h>
#include <mini/collections/linked_list.h>
#include <mini/collections/hashset.h>
#include <mini/collections/hashmap.h>
#include <mini/string.h>
#include <mini/console.h>

#include <mini/win32/pe.h>
#include <mini/win32/api_set.h>

#include <winternl.h>

int __cdecl
main(
  int argc,
  char* argv[]
  )
{
  MINI_UNREFERENCED(argc);
  MINI_UNREFERENCED(argv);

  auto apiset_namespace = reinterpret_cast<mini::win32::api_set_namespace_t*>(NtCurrentTeb()->ProcessEnvironmentBlock->Reserved9[0]);

  mini::win32::api_set_t apiset(apiset_namespace);

//   mini::string_ref dll_name = "api-ms-win-core-interlocked-l1-1-0.dll";
//   mini::string_ref api_set_name = apiset.dll_to_api_set_name(dll_name);
//
//   auto values =  apiset.find(api_set_name);
//
//   for (auto e2 : values)
//   {
//     mini::console::write("'%.*S' -> '%.*S' (0x%08x)\n", e2.name().size, e2.name().buffer, e2.value().size, e2.value().buffer, e2.flags());
//   }

  mini::console::write("=-=-=-=-= api_set =-=-=-=-=\n");

  for (auto e : apiset.enumerator())
  {
    mini::console::write("  name: '%.*S' (%i)\n", e.name().size, e.name().buffer, e.values().count());

    for (auto e2 : e.values())
    {
      mini::console::write("    flags: 0x%08x, value: '%.*S', name: '%.*S'\n", e2.flags(), e2.value().size, e2.value().buffer, e2.name().size, e2.name().buffer);
    }
  }

  LoadLibrary(TEXT("kernel32.dll"));
  auto pe = mini::win32::pe_file(GetModuleHandle(TEXT("kernel32.dll")));

  mini::console::write("=-=-=-=-= sections (%i) =-=-=-=-=\n", pe.sections().count());
  for (auto e : pe.sections())
  {
    mini::console::write("  name: '%.*s'\n", mini::win32::image_sizeof_short_name, e.name());
  }

  mini::console::write("=-=-=-=-= export_directory (%i) =-=-=-=-=\n", pe.export_directory().count());
  for (auto e : pe.export_directory())
  {
    mini::console::write("  name: '%s', ordinal: 0x%04x, is_forwarder: %i\n", e.name(), e.ordinal(), e.is_forwarder());
    if (e.is_forwarder())
    {
      mini::console::write("  >> '%s'\n", e.forwarder());
    }
    else
    {
      mini::console::write("  >> 0x%p\n", e.function());
    }
  }

  mini::console::write("=-=-=-=-= import_directory (%i) =-=-=-=-=\n", pe.import_directory().count());
  for (auto e : pe.import_directory())
  {
    mini::console::write("  name: '%s' (%i)\n", e.name(), e.import_thunks().count());

    if (apiset.is_api_set_name(e.name()))
    {
      mini::console::write("    | APISET\n");
      auto apiset_values = apiset.find(apiset.dll_to_api_set_name(e.name()));
      for (auto v : apiset_values)
      {
        mini::console::write("    | flags: 0x%08x, value: '%.*S', name: '%.*S'\n", v.flags(), v.value().size, v.value().buffer, v.name().size, v.name().buffer);
      }
    }

    for (auto t : e.import_thunks())
    {
      if (t.is_ordinal())
      {
        mini::console::write("    ordinal: 0x%04x\n", t.ordinal());
      }
      else
      {
        mini::console::write("    name: '%s', hint: %i\n", t.name(), t.hint());
      }
    }
  }

  mini::console::write("=-=-=-=-= resource_directory (%i) =-=-=-=-=\n", pe.resource_directory().count());
  for (auto e : pe.resource_directory())
  {
    if (e.name_is_string())  mini::console::write("  name: '%.*S'\n", e.name().size / 2, e.name().buffer);
    else                     mini::console::write("  id: %i\n", e.id());

    if (!e.data_is_directory())
    {
      mini::console::write("      code_page: %i, buffer: 0x%p, size: %i\n", e.data().code_page, e.data().buffer.get_buffer(), e.data().buffer.get_size());
    }
    else
    {
      for (auto e2 : e.resource_directory())
      {
        if (e2.name_is_string()) mini::console::write("    name: '%.*S'\n", e2.name().size / 2, e2.name().buffer);
        else                     mini::console::write("    id: %i\n", e2.id());

        if (!e2.data_is_directory())
        {
          mini::console::write("      code_page: %i, buffer: 0x%p, size: %i\n", e2.data().code_page, e2.data().buffer.get_buffer(), e2.data().buffer.get_size());
        }
        else
        {
          for (auto e3 : e2.resource_directory())
          {
            if (e3.name_is_string()) mini::console::write("      name: '%.*S'\n", e3.name().size / 2, e3.name().buffer);
            else                     mini::console::write("      id: %i\n", e3.id());

            if (!e3.data_is_directory())
            {
              mini::console::write("      code_page: %i, buffer: 0x%p, size: %i\n", e3.data().code_page, e3.data().buffer.get_buffer(), e3.data().buffer.get_size());
            }
          }
        }
      }
    }
  }

  mini::console::write("=-=-=-=-= relocation_directory =-=-=-=-=\n");
  for (auto e : pe.relocation_directory())
  {
    mini::console::write("  rva: 0x%p, va: 0x%p (%i)\n", e.relative_virtual_address(), e.virtual_address(), e.fixups().count());

    for (auto e2 : e.fixups())
    {
      mini::console::write("    rva: 0x%p, va: 0x%p, type: %i\n", e2.relative_virtual_address(), e2.virtual_address(), e2.type());
    }
  }

  mini::console::write("=-=-=-=-= tls_directory (%i) =-=-=-=-=\n", pe.tls_directory().count());
  for (auto e : pe.tls_directory())
  {
    mini::console::write("  tls_callback: 0x%p\n", e.tls_callback());
  }

  return 0;
}

#endif
