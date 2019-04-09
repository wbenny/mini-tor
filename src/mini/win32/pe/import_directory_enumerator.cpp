#include "import_directory_enumerator.h"
#include "../pe.h"

namespace mini::win32 {

template class pe_import_directory_enumerator<image_32_t>;
template class pe_import_directory_enumerator<image_64_t>;

template <
  typename TImageTraits
>
pe_import_directory_enumerator<TImageTraits>::pe_import_directory_enumerator(
  const pe_file* pe
  )
  : _pe(pe)
{
  _import_descriptor = _pe->directory_entry<typename TImageTraits::image_import_descriptor_t>();

  _count = 0;

  if (_import_descriptor.size > 0)
  {
    for (
      auto current_import_descriptor = _import_descriptor.entry;
      current_import_descriptor->name && current_import_descriptor->original_first_thunk;
      current_import_descriptor += 1
      )
    {
      _count += 1;
    }
  }
}

template<
  typename TImageTraits
>
pe_import_thunk_enumerator<TImageTraits>
pe_import_directory_enumerator<TImageTraits>::proxy::import_thunks(
  void
  ) const
{
  return pe_import_thunk_enumerator<TImageTraits>(_image_base, _import_descriptor_entry);
}

template <
  typename TImageTraits
>
uint32_t
pe_import_directory_enumerator<TImageTraits>::characteristics(
  void
  ) const
{
  return _import_descriptor.entry->characteristics;
}

template <
  typename TImageTraits
>
pe_import_directory_enumerator<TImageTraits>::iterator::iterator(
  const pe_import_directory_enumerator* enumerator,
  size_t index /*= 0 */
  )
  : _enumerator(enumerator)
  , _index(index)
{

}

template <
  typename TImageTraits
>
typename pe_import_directory_enumerator<TImageTraits>::iterator::value_type
pe_import_directory_enumerator<TImageTraits>::iterator::value(
  void
  ) const
{
  value_type result;

  result._image_base = _enumerator->_pe->_image_base;
  result._import_descriptor_entry = &_enumerator->_import_descriptor.entry[_index];

  return result;
}

}
