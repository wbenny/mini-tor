#pragma once
#include "import_thunk_enumerator.h"
#include "../pe.h"

namespace mini::win32 {

template class pe_import_thunk_enumerator<image_32_t>;
template class pe_import_thunk_enumerator<image_64_t>;

template <
  typename TImageTraits
>
pe_import_thunk_enumerator<TImageTraits>::pe_import_thunk_enumerator(
  uint8_t* image_base,
  typename TImageTraits::image_import_descriptor_t* import_descriptor
  )
  : _image_base(image_base)
{
  _name_thunk_data    = reinterpret_cast<typename TImageTraits::image_thunk_data_t*>(image_base + import_descriptor->original_first_thunk);
  _address_thunk_data = reinterpret_cast<typename TImageTraits::image_thunk_data_t*>(image_base + import_descriptor->first_thunk);

  _count = 0;
  for (
    auto current_name_thunk_data = _name_thunk_data;
    current_name_thunk_data->address_of_data;
    current_name_thunk_data += 1
    )
  {
    _count += 1;
  }
}

template <
  typename TImageTraits
>
pe_import_thunk_enumerator<TImageTraits>::iterator::iterator(
  const pe_import_thunk_enumerator* enumerator,
  size_t index /*= 0 */
  )
  : _enumerator(enumerator)
  , _index(index)
{

}

template <
  typename TImageTraits
>
typename pe_import_thunk_enumerator<TImageTraits>::iterator::value_type
pe_import_thunk_enumerator<TImageTraits>::iterator::value(
  void
  ) const
{
  value_type result;

  result._image_base = _enumerator->_image_base;
  result._name_thunk_data = &_enumerator->_name_thunk_data[_index];

  return result;
}

}


