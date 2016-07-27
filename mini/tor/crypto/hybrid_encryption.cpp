#include "hybrid_encryption.h"

#include <mini/crypto/random.h>
#include <mini/crypto/rsa.h>
#include <mini/crypto/aes.h>

namespace mini::tor::hybrid_encryption {

byte_buffer
encrypt(
  const byte_buffer_ref data,
  const byte_buffer_ref public_key
  )
{
  if (data.get_size() < PK_DATA_LEN)
  {
    return crypto::rsa::public_encrypt(data, public_key);
  }

  byte_buffer random_key = crypto::provider_factory.create_random()->get_random_bytes(KEY_LEN);

  //
  // RSA( K | M1 ) --> C1
  //
  byte_buffer k_and_m1;
  k_and_m1.reserve(PK_DATA_LEN);

  k_and_m1.add_many(random_key);
  k_and_m1.add_many(data.slice(0, PK_DATA_LEN_WITH_KEY));

  auto c1 = crypto::rsa::public_encrypt(k_and_m1, public_key);

  //
  // AES_CTR(M2)  --> C2
  //
  byte_buffer_ref m2 = data.slice(PK_DATA_LEN_WITH_KEY);
  auto c2 = crypto::aes::crypt(crypto::aes::mode::mode_ctr, crypto::aes::key_size::key_size_128, random_key, m2);

  //
  // C1 | C2
  //
  byte_buffer result;
  result.reserve(c1.get_size() + c2.get_size());

  result.add_many(c1);
  result.add_many(c2);

  return result;
}

}
