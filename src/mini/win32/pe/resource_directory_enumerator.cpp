#include "resource_directory_enumerator.h"
#include "../pe.h"

namespace mini::win32 {

template class pe_resource_directory_enumerator<image_32_t>;
template class pe_resource_directory_enumerator<image_64_t>;

template <
  typename TImageTraits
>
pe_resource_directory_enumerator<TImageTraits>::pe_resource_directory_enumerator(
  const pe_file* pe
  )
  : _pe(pe)
{
  _resource_directory = _pe->directory_entry<typename TImageTraits::image_resource_directory_t>();

  _count = _resource_directory.size > 0
    ? _resource_directory.entry->number_of_named_entries + _resource_directory.entry->number_of_id_entries
    : 0;

  _top_resource_directory = _resource_directory;
}

template <
  typename TImageTraits
>
pe_resource_directory_enumerator<TImageTraits>::pe_resource_directory_enumerator(
  const pe_file* pe,
  pe_directory<typename TImageTraits::image_resource_directory_t> resource_directory,
  pe_directory<typename TImageTraits::image_resource_directory_t> top_resource_directory
  )
  : _pe(pe)
{
  _resource_directory = resource_directory;

  _count = _resource_directory.size > 0
    ? _resource_directory.entry->number_of_named_entries + _resource_directory.entry->number_of_id_entries
    : 0;

  _top_resource_directory = top_resource_directory;
}

template <
  typename TImageTraits
>
uint32_t
pe_resource_directory_enumerator<TImageTraits>::characteristics(
  void
  ) const
{
  return _resource_directory.entry->characteristics;
}

template <
  typename TImageTraits
>
pe_resource_directory_enumerator<TImageTraits>::iterator::iterator(
  const pe_resource_directory_enumerator* enumerator,
  size_t index /*= 0 */
  )
  : _enumerator(enumerator)
  , _index(index)
{
  _resource_entry_table = reinterpret_cast<typename TImageTraits::image_resource_directory_entry_t*>(_enumerator->_resource_directory.entry + 1);
}

template <
  typename TImageTraits
>
typename pe_resource_directory_enumerator<TImageTraits>::iterator::value_type
pe_resource_directory_enumerator<TImageTraits>::iterator::value(
  void
  ) const
{
  value_type result;

  result._pe = _enumerator->_pe;
  result._top_resource_directory = _enumerator->_top_resource_directory;
  result._resource_directory_entry = &_resource_entry_table[_index];

  return result;
}

}
