#include "key_agreement.h"

#include <mini/crypto/random.h>

namespace mini::tor {

key_agreement::key_agreement(
  void
  )
{
  generate_key_pair();
}

void
key_agreement::generate_key_pair(
  void
  )
{
  _dh_private_key = dh1024::generate_private_key(DH_G, DH_P);
}

byte_buffer
key_agreement::get_public_key(
  void
  ) const
{
  return _dh_private_key.export_public_key().get_y();
}

byte_buffer
key_agreement::get_private_key(
  void
  ) const
{
  return _dh_private_key.get_exponent();
}

byte_buffer
key_agreement::get_shared_secret(
  const byte_buffer_ref other_public_key
  ) const
{
  return _dh_private_key.get_shared_secret(other_public_key);
}

}
