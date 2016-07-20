#include "provider.h"

#include <mini/crypto/aes.h>
#include <mini/crypto/sha1.h>
#include <mini/crypto/rsa.h>
#include <mini/crypto/random.h>

namespace mini::crypto {

provider provider_factory;

provider::provider(
  void
  )
  : _provider_handle(0)
{
  init();
}

provider::~provider(
  void
  )
{
  destroy();
}

void
provider::init(
  void
  )
{
  CryptAcquireContext(
    &_provider_handle,
    NULL,
    MS_ENH_RSA_AES_PROV,
    PROV_RSA_AES,
    CRYPT_VERIFYCONTEXT);
}

void
provider::destroy(
  void
  )
{
  CryptReleaseContext(
    _provider_handle,
    0);

  _provider_handle = 0;
}

HCRYPTPROV
provider::get_handle(
  void
  )
{
  return _provider_handle;
}

ptr<aes>
provider::create_aes(
  void
  )
{
  return new aes(this);
}

ptr<sha1>
provider::create_sha1(
  void
  )
{
  return new sha1(this);
}

ptr<rsa>
provider::create_rsa(
  void
  )
{
  return new rsa(this);
}

ptr<random>
provider::create_random(
  void
  )
{
  return new random(this);
}

}
