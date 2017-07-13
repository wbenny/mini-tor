#pragma once
#include <mini/common.h>

#include <windows.h>

namespace mini::io {

class file_attributes
{
  public:
    static const file_attributes invalid;

    file_attributes(
      void
      );

    file_attributes(
      uint32_t attributes
      );

    file_attributes(
      const file_attributes& other
      ) = default;

    file_attributes(
      file_attributes&& other
      ) = default;

    ~file_attributes(
      void
      ) = default;

    file_attributes&
    operator=(
      const file_attributes& other
      ) = default;

    file_attributes&
    operator=(
      file_attributes&& other
      ) = default;

    bool
    operator==(
      const file_attributes& other
      ) const;

    bool
    operator!=(
      const file_attributes& other
      ) const;

    bool
    is_directory(
      void
      ) const;

    bool
    is_file(
      void
      ) const;

    bool
    is_system(
      void
      ) const;

    bool
    is_hidden(
      void
      ) const;

  private:
    uint32_t _attributes;
};

}
