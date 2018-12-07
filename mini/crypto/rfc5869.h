#pragma once
#include "hmac_sha256.h"

#include <mini/byte_buffer.h>
#include <mini/algorithm.h>

#include <type_traits>

namespace mini::crypto {

template <
  typename HMAC_CLASS
>
class rfc5869
{
  MINI_MAKE_NONCONSTRUCTIBLE(rfc5869);

  public:
    // static_assert(std::is_base_of_v<crypto::hmac_base, HMAC_CLASS>, "HMAC_CLASS must be derived from hmac_base!");

    static
    byte_buffer
    derive_key(
      const byte_buffer_ref initial_key_material,
      const byte_buffer_ref info,
      const byte_buffer_ref salt,
      size_type output_size
      )
    {
      return expand(HMAC_CLASS::compute(salt, initial_key_material), info, output_size);
    }

  private:
    static
    byte_buffer
    expand(
      const byte_buffer_ref pseudo_random_key,
      const byte_buffer_ref info,
      size_type output_size
      )
    {
      mini_assert(output_size <= 255 * HMAC_CLASS::hash_size_in_bytes);

      byte_buffer result;
      byte_buffer result_block;
      size_type bytes_remaining = output_size;

      for (byte_type i = 1; bytes_remaining > 0; i++)
      {
        byte_buffer message;
        message.add_many(result_block);
        message.add_many(info);
        message.add(i);

        result_block = HMAC_CLASS::compute(pseudo_random_key, message);

        size_type bytes_processed = algorithm::min(result_block.get_size(), bytes_remaining);
        result.add_many(result_block.slice(0, bytes_processed));

        bytes_remaining -= bytes_processed;
      }

      return result;
    }
};

}
