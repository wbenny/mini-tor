#pragma once
#include "section_enumerator.h"
#include "../pe.h"

namespace mini::win32 {

template class pe_section_enumerator<image_32_t>;
template class pe_section_enumerator<image_64_t>;

template <
  typename TImageTraits
>
pe_section_enumerator<TImageTraits>::pe_section_enumerator(
  const pe_file* pe
  )
  : _pe(pe)
{
  _section_table = reinterpret_cast<image_section_header_t*>(
    reinterpret_cast<uint8_t*>(&_pe->nt_headers()) +
    offsetof(image_nt_headers_32_t, optional_header) +
    _pe->nt_headers().file_header.size_of_optional_header
    );

  _count = _pe->nt_headers().file_header.number_of_sections;
}

template <
  typename TImageTraits
>
pe_section_enumerator<TImageTraits>::iterator::iterator(
  const pe_section_enumerator* enumerator,
  size_t index /*= 0 */
  )
  : _enumerator(enumerator)
  , _index(index)
{

}

template <
  typename TImageTraits
>
typename pe_section_enumerator<TImageTraits>::iterator::reference
pe_section_enumerator<TImageTraits>::iterator::value(
  void
  ) const
{
  value_type result;

  result._image_base = _enumerator->_pe->_image_base;
  result._section_header = &_enumerator->_section_table[_index];

  return result;
}

}
