#pragma once
#include "hash_base.h"

namespace mini::crypto {

template <
  ALG_ID ALGORITHM_ID,
  size_type HASH_SIZE
>
class hash_template_base
  : public hash_base
{
  public:
    static constexpr size_type hash_size          = HASH_SIZE;
    static constexpr size_type hash_size_in_bytes = hash_size / 8;

    struct hash_type
    {
      byte_type value[hash_size_in_bytes];
    };

    hash_template_base(
      void
      );

    hash_template_base(
      const hash_template_base& other
      );

    hash_template_base(
      hash_template_base&& other
      );

    hash_template_base&
    operator=(
      const hash_template_base& other
      );

    hash_template_base&
    operator=(
      hash_template_base&& other
      );

    hash_template_base
    duplicate(
      void
      );

    size_type
    get_hash_size(
      void
      ) const override;

    static byte_buffer
    hash(
      const byte_buffer_ref input
      );
};

}

#include "hash_template_base.inl"
