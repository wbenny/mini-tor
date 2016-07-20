#pragma once
#include "string.h"

#include <cstring>

namespace mini {

class string_view
{
  public:
    string_view(const char* other)
      : _first(other)
      , _last(other + strlen(other) + 1)
    {
    
    }

    string_view(const string& other)
      : _first(other.get_buffer())
      , _last(other.get_buffer() + other.get_size() + 1)
    {

    }

    string_view(const string_view& other)
      : _first(other._first)
      , _last(other._last)
    {

    }

  private:
    const char* _first;
    const char* _last;
};

}
