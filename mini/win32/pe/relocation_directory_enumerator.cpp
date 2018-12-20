#pragma once
#include "relocation_directory_enumerator.h"
#include "../pe.h"

namespace mini::win32 {

template class pe_relocation_directory_enumerator<image_32_t>;
template class pe_relocation_directory_enumerator<image_64_t>;

template <
  typename TImageTraits
>
pe_relocation_directory_enumerator<TImageTraits>::pe_relocation_directory_enumerator(
  const pe_file* pe
  )
  : _pe(pe)
{
  _base_relocation = _pe->directory_entry<typename TImageTraits::image_base_relocation_t>();
  _base_relocation_end = reinterpret_cast<typename TImageTraits::image_base_relocation_t*>(uintptr_t(_base_relocation.entry) + _base_relocation.size);
}

template <
  typename TImageTraits
>
pe_relocation_directory_enumerator<TImageTraits>::iterator::iterator(
  const pe_relocation_directory_enumerator* enumerator,
  typename TImageTraits::image_base_relocation_t* current_relocation
  )
  : _enumerator(enumerator)
  , _relocation_current(current_relocation)
  , _relocation_end(enumerator->_base_relocation_end)
{

}

template <
  typename TImageTraits
>
void
pe_relocation_directory_enumerator<TImageTraits>::iterator::advance(
  void
  )
{
  _relocation_current = reinterpret_cast<image_base_relocation_t*>(
    uintptr_t(_relocation_current) + _relocation_current->size_of_block
    );

  if (_relocation_current > _relocation_end)
  {
    _relocation_current = _relocation_end;
  }
}

template <
  typename TImageTraits
>
typename pe_relocation_directory_enumerator<TImageTraits>::iterator::value_type
pe_relocation_directory_enumerator<TImageTraits>::iterator::value(
  void
  ) const
{
  value_type result;

  result._image_base = _enumerator->_pe->_image_base;
  result._relocation = _relocation_current;

  return result;
}

}
