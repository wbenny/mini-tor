#pragma once
#include <mini/string.h>

namespace mini::io {

class path
{
  public:
    static constexpr auto directory_separator             = string_ref("\\");
    static constexpr auto alternative_directory_separator = string_ref("/");
    static constexpr auto extension_separator             = string_ref(".");

    static string
    combine(
      const string_ref p1,
      const string_ref p2
      );

    template <
      typename ...ARGS
    >
    static string
    combine(
      const string_ref p1,
      const string_ref p2,
      ARGS... pX
      )
    {
      //
      // TODO:
      // ARGS should be verified to be convertible to string_ref.
      //
      return combine(combine(p1, p2), pX...);
    }

    static string_collection
    split(
        const string_ref p
      );

    static string_ref
    get_file_name(
      const string_ref p
      );

    static string_ref
    get_directory_name(
      const string_ref p
      );

    static string_ref
    get_filename_without_extension(
      const string_ref p
      );

    static string_ref
    get_extension(
      const string_ref p
      );
};

}
