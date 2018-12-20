#pragma once
#include "tls_directory_enumerator.h"
#include "../pe.h"

namespace mini::win32 {

template class pe_tls_directory_enumerator<image_32_t>;
template class pe_tls_directory_enumerator<image_64_t>;

template <
  typename TImageTraits
>
pe_tls_directory_enumerator<TImageTraits>::pe_tls_directory_enumerator(
  const pe_file* pe
  )
  : _pe(pe)
{
  _tls_directory = _pe->directory_entry<typename TImageTraits::image_tls_directory_t>();
  _count = 0;

  if (_tls_directory.size > 0)
  {
    _tls_callback_table = reinterpret_cast<image_tls_callback_fn_t*>(uintptr_t(_tls_directory.entry->address_of_callbacks));

    for (
      auto current_tls_callback = _tls_callback_table;
      *current_tls_callback;
      current_tls_callback += 1
      )
    {
      _count += 1;
    }
  }
}

template <
  typename TImageTraits
>
uint32_t
pe_tls_directory_enumerator<TImageTraits>::characteristics(
  void
  ) const
{
  return _tls_directory.entry->characteristics;
}

template <
  typename TImageTraits
>
void*
pe_tls_directory_enumerator<TImageTraits>::data(
  void
  ) const
{
  return reinterpret_cast<void*>(_pe->_image_base + _tls_directory.entry->start_address_of_raw_data);
}

template <
  typename TImageTraits
>
size_t
pe_tls_directory_enumerator<TImageTraits>::data_size(
  void
  ) const
{
  return
    reinterpret_cast<uint8_t*>(_pe->_image_base + _tls_directory.entry->end_address_of_raw_data) -
    reinterpret_cast<uint8_t*>(_pe->_image_base + _tls_directory.entry->start_address_of_raw_data);
}

template <
  typename TImageTraits
>
pe_tls_directory_enumerator<TImageTraits>::iterator::iterator(
  const pe_tls_directory_enumerator* enumerator,
  size_t index /*= 0 */
  )
  : _enumerator(enumerator)
  , _index(index)
{

}

template <
  typename TImageTraits
>
typename pe_tls_directory_enumerator<TImageTraits>::iterator::value_type
pe_tls_directory_enumerator<TImageTraits>::iterator::value(
  void
  ) const
{
  value_type result;

  result._tls_callback = _enumerator->_tls_callback_table[_index];

  return result;
}

}
