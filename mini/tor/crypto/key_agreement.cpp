#include "key_agreement.h"

#include <mini/crypto/random.h>

namespace mini::tor {

key_agreement::key_agreement(
  size_t private_key_bytes
  )
{
  _dh = crypto::provider_factory.create_dh1024();
  generate_key_pair(private_key_bytes);
}

void
key_agreement::generate_key_pair(
  size_t private_key_bytes
  )
{
  _dh->generate_key(DH_G, DH_P);
}

byte_buffer
key_agreement::get_public_key(
  void
  ) const
{
  return _dh->get_public_key();
}

byte_buffer
key_agreement::get_private_key(
  void
  ) const
{
  return _dh->get_private_key();
}

byte_buffer
key_agreement::get_shared_secret(
  const byte_buffer_ref other_public_key
  ) const
{
  return _dh->get_shared_secret(other_public_key);
}

}
