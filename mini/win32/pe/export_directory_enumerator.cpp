#pragma once
#include "export_directory_enumerator.h"
#include "../pe.h"

namespace mini::win32 {

template class pe_export_directory_enumerator<image_32_t>;
template class pe_export_directory_enumerator<image_64_t>;

template <
  typename TImageTraits
>
pe_export_directory_enumerator<TImageTraits>::pe_export_directory_enumerator(
  const pe_file* pe
  )
  : _pe(pe)
{
  _export_directory = _pe->directory_entry<typename TImageTraits::image_export_directory_t>();

  _count = _export_directory.size > 0
    ? _export_directory.entry->number_of_names
    : 0;
}

template <
  typename TImageTraits
>
uint32_t
pe_export_directory_enumerator<TImageTraits>::characteristics(
  void
  ) const
{
  return _export_directory.entry->characteristics;
}

template <
  typename TImageTraits
>
pe_export_directory_enumerator<TImageTraits>::iterator::iterator(
  const pe_export_directory_enumerator* enumerator,
  size_t index /*= 0 */
  )
  : _enumerator(enumerator)
  , _index(index)
{
  _function_table = reinterpret_cast<uint32_t*>(_enumerator->_pe->_image_base + _enumerator->_export_directory.entry->address_of_functions);
  _name_table     = reinterpret_cast<uint32_t*>(_enumerator->_pe->_image_base + _enumerator->_export_directory.entry->address_of_names);
  _ordinal_table  = reinterpret_cast<uint16_t*>(_enumerator->_pe->_image_base + _enumerator->_export_directory.entry->address_of_name_ordinals);
}

template <
  typename TImageTraits
>
typename pe_export_directory_enumerator<TImageTraits>::iterator::value_type
pe_export_directory_enumerator<TImageTraits>::iterator::value(
  void
  ) const
{
  value_type result;

  result._image_base        = _enumerator->_pe->_image_base;
  result._export_directory  = _enumerator->_export_directory;
  result._function          = _function_table[_index];
  result._name              = _name_table[_index];
  result._ordinal           = _ordinal_table[_index];

  return result;
}

}
