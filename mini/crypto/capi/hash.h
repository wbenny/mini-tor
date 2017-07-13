#pragma once
#include "../common.h"
#include <mini/byte_buffer.h>

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto::capi {

template <
  hash_algorithm_type HASH_ALGORITHM
>
class hash
{
  public:
    static constexpr size_type hash_size          = hash_algorithm_to_bit_size(HASH_ALGORITHM);
    static constexpr size_type hash_size_in_bytes = hash_size / 8;
    static constexpr hash_algorithm_type hash_algorithm = HASH_ALGORITHM;

    //
    // constructors.
    //

    hash(
      void
      );

    hash(
      const hash& other
      );

    hash(
      hash&& other
      );

    //
    // destructor.
    //

    ~hash(
      void
      );

    //
    // assign operators.
    //

    hash&
    operator=(
      const hash& other
      );

    hash&
    operator=(
      hash&& other
      );

    hash
    duplicate(
      void
      );

    //
    // swap.
    //

    void
    swap(
      hash& other
      );

    //
    // operations.
    //

    void
    update(
      const byte_buffer_ref input
      );

    static byte_buffer
    compute(
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

  protected:
    hash(
      ALG_ID alg_id,
      const byte_buffer_ref key,
      HCRYPTKEY& key_handle
      );

    void
    init(
      ALG_ID alg_id
      );

    void
    init(
      ALG_ID alg_id,
      const byte_buffer_ref key,
      HCRYPTKEY& key_handle
      );

    void
    destroy(
      void
      );

    void
    duplicate_internal(
      const hash& other
      );

  private:
    HCRYPTHASH _hash_handle = 0;
};

}

#include "hash.inl"
