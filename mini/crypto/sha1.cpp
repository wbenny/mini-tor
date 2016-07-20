#include "sha1.h"
#include "provider.h"

namespace mini::crypto {

sha1::sha1(
  provider* crypto_provider
  )
  : _provider(crypto_provider)
{
  init();
}

sha1::sha1(
  provider* crypto_provider,
  const sha1& other
  )
  : _provider(crypto_provider)
{
  duplicate_internal(other);
}

sha1::~sha1(
  void
  )
{
  destroy();
}

void
sha1::init(
  void
  )
{
  CryptCreateHash(
    _provider->get_handle(),
    CALG_SHA1,
    0,
    0,
    &_hash);
}

void
sha1::destroy(
  void
  )
{
  CryptDestroyHash(_hash);
}

void
sha1::update(
  const byte_buffer_ref input
  )
{
  BOOL result;
  result = CryptHashData(
    _hash,
    input.get_buffer(),
    (DWORD)input.get_size(),
    0);
}

void
sha1::get(
  uint8_t output[20]
  )
{
  DWORD hash_size = 20;
  CryptGetHashParam(
    _hash,
    HP_HASHVAL,
    output,
    &hash_size,
    0);
}

byte_buffer
sha1::get(
  void
  )
{
  byte_buffer result(20);
  get(&result[0]);

  return result;
}

ptr<sha1>
sha1::duplicate(
  void
  ) const
{
  return new sha1(_provider, *this);
}

void
sha1::duplicate_internal(
  const sha1& other
  )
{
  CryptDuplicateHash(
    other._hash,
    NULL,
    0,
    &_hash);
}

byte_buffer
sha1::hash(
  const byte_buffer_ref input
  )
{
  ptr<sha1> md = provider_factory.create_sha1();
  md->update(input);
  return md->get();
}

}
