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
  using aes128_ctr = crypto::aes_ctr<128>;

  if (data.get_size() < PK_DATA_LEN)
  {
    return rsa1024::public_encrypt(
      rsa1024::public_key::make_from_der(public_key),
      data,
      rsa1024::encryption_padding::oaep_sha1);
  }

  byte_buffer random_key = crypto::random_device.get_random_bytes(KEY_LEN);

  //
  // RSA( K | M1 ) --> C1
  //
  byte_buffer k_and_m1;
  k_and_m1.reserve(PK_DATA_LEN);

  k_and_m1.add_many(random_key);
  k_and_m1.add_many(data.slice(0, PK_DATA_LEN_WITH_KEY));

  auto c1 = rsa1024::public_encrypt(
    rsa1024::public_key::make_from_der(public_key),
    k_and_m1,
    rsa1024::encryption_padding::oaep_sha1);

  //
  // AES_CTR(M2)  --> C2
  //
  byte_buffer_ref m2 = data.slice(PK_DATA_LEN_WITH_KEY);
  auto c2 = aes128_ctr::crypt(aes128_ctr::key(random_key), m2);

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
