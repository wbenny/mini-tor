#include "api_set_value_enumerator.h"
#include "../api_set.h"

namespace mini::win32 {

api_set_value_enumerator::api_set_value_enumerator(
  uint8_t* api_set_namespace_base,
  api_set_value_entry_t* api_set_value_table,
  size_t count
  )
  : _api_set_namespace_base(api_set_namespace_base)
  , _api_set_value_table(api_set_value_table)
{
  _count = count;
}


api_set_value_enumerator::iterator::iterator(
  const api_set_value_enumerator* enumerator,
  size_t index /*= 0 */
  )
  : _enumerator(enumerator)
  , _index(index)
{

}

api_set_value_enumerator::iterator::value_type
api_set_value_enumerator::iterator::value(
  void
  ) const
{
  auto& entry = reinterpret_cast<api_set_value_entry_t*>(_enumerator->_api_set_value_table)[_index];

  value_type result;

  //
  // name_offset can be 0.
  //

  result._name         = entry.name_offset ?
    reinterpret_cast<wchar_t*>(_enumerator->_api_set_namespace_base + entry.name_offset)
    : nullptr;
  result._name_length  = entry.name_length / sizeof(wchar_t);
  result._value        = reinterpret_cast<wchar_t*>(_enumerator->_api_set_namespace_base + entry.value_offset);
  result._value_length = entry.value_length / sizeof(wchar_t);
  result._flags        = entry.flags;

  return result;  
}

}

