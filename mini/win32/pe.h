#pragma once
#include "pe/section_enumerator.h"
#include "pe/export_directory_enumerator.h"
#include "pe/import_directory_enumerator.h"
#include "pe/resource_directory_enumerator.h"
#include "pe/relocation_directory_enumerator.h"
#include "pe/tls_directory_enumerator.h"

#include "pe/common.h"

#include <cstdint>
#include <iterator>

namespace mini::win32 {

class pe_file
{
  template <typename> friend class pe_section_enumerator;
  template <typename> friend class pe_export_directory_enumerator;
  template <typename> friend class pe_import_directory_enumerator;
  template <typename> friend class pe_import_thunk_enumerator;
  template <typename> friend class pe_resource_directory_enumerator;
  template <typename> friend class pe_relocation_directory_enumerator;
  template <typename> friend class pe_relocation_fixup_enumerator;
  template <typename> friend class pe_tls_directory_enumerator;

  public:
    pe_file() = delete;
    pe_file(const pe_file& other) = default;
    pe_file(pe_file&& other) = default;
    pe_file& operator=(const pe_file& other) = default;
    pe_file& operator=(pe_file&& other) = default;
    ~pe_file() = default;

    pe_file(void* image_base, size_t image_size = size_t(0))
      : _image_base(reinterpret_cast<uint8_t*>(image_base))
      , _image_size(image_size)
    { }

    void* image_base() const
    { return _image_base; }

    size_t image_size() const
    { return _image_size; }

    image_dos_header_t& dos_header() const
    { return reinterpret_cast<image_dos_header_t&>(*_image_base); }

    image_nt_headers_t& nt_headers() const
    { return reinterpret_cast<image_nt_headers_t&>(*(_image_base + dos_header().e_lfanew)); }

    image_nt_headers_32_t& nt_headers_32() const
    { return reinterpret_cast<image_nt_headers_32_t&>(*(_image_base + dos_header().e_lfanew)); }

    image_nt_headers_64_t& nt_headers_64() const
    { return reinterpret_cast<image_nt_headers_64_t&>(*(_image_base + dos_header().e_lfanew)); }

    image_file_machine_t machine() const
    { return image_file_machine_t { nt_headers().file_header.machine }; }

    image_file_characteristics_t characteristics() const
    { return image_file_characteristics_t { image_file_flags_t { nt_headers().file_header.characteristics } }; }

    bool is_32_bit() const
    { return  machine() == image_file_machine_t::i386
           || machine() == image_file_machine_t::arm; }
    //{ return !!(nt_headers().file_header.characteristics & image_file_32bit_machine); }

    bool is_64_bit() const
    { return  machine() == image_file_machine_t::amd64
           || machine() == image_file_machine_t::arm64
           || machine() == image_file_machine_t::alpha64
           || machine() == image_file_machine_t::ia64; }
    //{ return !is_32_bit(); }

    template <typename TImageTraits = image_t>
    pe_section_enumerator<TImageTraits> sections() const
    { return pe_section_enumerator<TImageTraits>(this); }

    template <typename TImageTraits = image_t>
    pe_export_directory_enumerator<TImageTraits> export_directory() const
    { return pe_export_directory_enumerator<TImageTraits>(this); }

    template <typename TImageTraits = image_t>
    pe_import_directory_enumerator<TImageTraits> import_directory() const
    { return pe_import_directory_enumerator<TImageTraits>(this); }

    template <typename TImageTraits = image_t>
    pe_resource_directory_enumerator<TImageTraits> resource_directory() const
    { return pe_resource_directory_enumerator<TImageTraits>(this); }

    template <typename TImageTraits = image_t>
    pe_relocation_directory_enumerator<TImageTraits> relocation_directory() const
    { return pe_relocation_directory_enumerator<TImageTraits>(this); }

    template <typename TImageTraits = image_t>
    pe_tls_directory_enumerator<TImageTraits> tls_directory() const
    { return pe_tls_directory_enumerator<TImageTraits>(this); }

    template <typename T>
    pe_directory<T> directory_entry() const
    {
      auto& entry = nt_headers().optional_header.data_directory[int(T::index)];

      return {
        reinterpret_cast<T*>(_image_base + entry.virtual_address),
                                           entry.size
      };
    }

    pe_directory<> directory_entry(image_directory_t directory) const
    {
      auto& entry = nt_headers().optional_header.data_directory[int(directory)];

      return {
        _image_base + entry.virtual_address,
                      entry.size
      };
    }

  private:
    uint8_t* _image_base;
    size_t _image_size;
};

}

