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
  using rsa1024 = crypto::rsa<1024>;
  using aes_ctr_128 = crypto::aes<crypto::cipher_mode::ctr, 128>;

  if (data.get_size() < PK_DATA_LEN)
  {
    return rsa1024::public_key::make_from_der(public_key).encrypt(
      data,
      crypto::rsa_encryption_padding::oaep_sha1,
      true);
  }

  byte_buffer random_key = crypto::random_device.get_random_bytes(KEY_LEN);

  //
  // RSA( K | M1 ) --> C1
  //
  byte_buffer k_and_m1 = { random_key, data.slice(0, PK_DATA_LEN_WITH_KEY) };

  auto c1 = rsa1024::public_key::make_from_der(public_key).encrypt(
    k_and_m1,
    crypto::rsa_encryption_padding::oaep_sha1,
    true);

  //
  // AES_CTR(M2)  --> C2
  //
  byte_buffer_ref m2 = data.slice(PK_DATA_LEN_WITH_KEY);
  auto c2 = aes_ctr_128::crypt(aes_ctr_128::key(random_key), m2);

  //
  // C1 | C2
  //
  byte_buffer result = { c1, c2 };

  return result;
}

}
