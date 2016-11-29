#pragma once
#include <mini/byte_buffer.h>

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto {

class provider;

class hash_base
{
  public:
    //
    // destructor.
    //

    virtual ~hash_base(
      void
      );

    //
    // assign operators.
    //

    hash_base&
    operator=(
      const hash_base& other
      );

    hash_base&
    operator=(
      hash_base&& other
      );

    //
    // swap.
    //

    void
    swap(
      hash_base& other
      );

    //
    // operations.
    //

    void
    update(
      const byte_buffer_ref input
      );

    //
    // accessors.
    //

    void
    get(
      mutable_byte_buffer_ref output
      );

    byte_buffer
    get(
      void
      );

    virtual size_type
    get_hash_size(
      void
      ) const = 0;

  protected:

    //
    // constructors.
    //

    hash_base(
      ALG_ID alg_id
      );

    hash_base(
      const hash_base& other
      );

    hash_base(
      hash_base&& other
      );

  private:
    void
    init(
      ALG_ID alg_id
      );

    void
    destroy(
      void
      );

    void
    duplicate_internal(
      const hash_base& other
      );

    HCRYPTHASH _hash_handle = 0;
};

}
