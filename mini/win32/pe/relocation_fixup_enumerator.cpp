#pragma once
#include "relocation_fixup_enumerator.h"
#include "../pe.h"

namespace mini::win32 {

template class pe_relocation_fixup_enumerator<image_32_t>;
template class pe_relocation_fixup_enumerator<image_64_t>;

template <
  typename TImageTraits
>
pe_relocation_fixup_enumerator<TImageTraits>::pe_relocation_fixup_enumerator(
  uint8_t* image_base,
  typename TImageTraits::image_base_relocation_t* relocation
  )
  : _image_base(image_base)
  , _relocation(relocation)
{
  _count = (relocation->size_of_block - sizeof(typename TImageTraits::image_base_relocation_t)) / sizeof(uint16_t);
}

template <
  typename TImageTraits
>
pe_relocation_fixup_enumerator<TImageTraits>::iterator::iterator(
  const pe_relocation_fixup_enumerator* enumerator,
  size_t index /*= 0 */
  )
  : _enumerator(enumerator)
  , _index(index)
{

}

template <
  typename TImageTraits
>
typename pe_relocation_fixup_enumerator<TImageTraits>::iterator::value_type
pe_relocation_fixup_enumerator<TImageTraits>::iterator::value(
  void
  ) const
{
  value_type result;

  result._image_base = _enumerator->_image_base;
  result._relocation_base = _enumerator->_relocation->virtual_address;
  result._fixup_item = reinterpret_cast<uint16_t*>(_enumerator->_relocation + 1) + _index;

  return result;
}

}
