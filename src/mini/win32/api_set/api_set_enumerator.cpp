#include "api_set_enumerator.h"
#include "../api_set.h"

namespace mini::win32 {

api_set_enumerator::api_set_enumerator(
  const api_set_t* api_set
  )
  : _api_set(api_set)
{
  _count = _api_set->_api_set_namespace->count;
}


api_set_enumerator::iterator::iterator(
  const api_set_enumerator* enumerator,
  size_t index /*= 0 */
  )
  : _enumerator(enumerator)
  , _index(index)
{

}

api_set_enumerator::iterator::value_type
api_set_enumerator::iterator::value(
  void
  ) const
{
  auto& entry = reinterpret_cast<api_set_namespace_entry_t*>(
    _enumerator->_api_set->_api_set_namespace_base + _enumerator->_api_set->_api_set_namespace->entry_offset
    )[_index];

  value_type result;
  result._name                    = reinterpret_cast<wchar_t*>(
    _enumerator->_api_set->_api_set_namespace_base + entry.name_offset
    );

  result._name_length             = entry.name_length;
  result._api_set_namespace_base = _enumerator->_api_set->_api_set_namespace_base;
  result._api_set_value_table    = reinterpret_cast<api_set_value_entry_t*>(
    _enumerator->_api_set->_api_set_namespace_base + entry.value_offset
    );

  result._api_set_value_count    = entry.value_count;
  return result;
}

}

