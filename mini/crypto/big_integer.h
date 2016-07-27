#pragma once
#include "impl/bigint_impl.h"

#include <mini/common.h>
#include <mini/byte_buffer.h>

namespace mini::crypto {

class big_integer
{
  public:
    static void
    global_init(
      void
      );

    static void
    global_destroy(
      void
      );

    //
    // constructors.
    //

    big_integer(
      void
      );

    big_integer(
      const big_integer& other
      );

    big_integer(
      big_integer&& other
      );

    big_integer(
      const byte_buffer_ref input
      );

    //
    // destructor.
    //

    ~big_integer(
      void
      );

    //
    // assign operators.
    //

    big_integer&
    operator=(
      const big_integer& other
    );

    big_integer&
    operator=(
      big_integer&& other
    );

    big_integer&
    operator=(
      const byte_buffer_ref input
      );

    //
    // conversion methods.
    //

    void
    from_bytes(
      const byte_buffer_ref input
      );

    byte_buffer
    to_bytes(
      void
      ) const;

    //
    // operations.
    //

    big_integer
    mod_pow(
      const big_integer& modulus,
      const big_integer& exponent
      ) const;

    //
    // conversion operators.
    //

    operator byte_buffer(
      void
      ) const;

  private:
    big_integer(
      ::bigint* impl
      );

    ::bigint* _impl;
};

}
