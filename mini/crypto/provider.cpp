#include "provider.h"

#include <mini/crypto/aes.h>
#include <mini/crypto/sha1.h>
#include <mini/crypto/dh1024.h>
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

  CryptAcquireContext(
    &_dh_provider_handle,
    NULL,
    MS_ENH_DSS_DH_PROV,
    PROV_DSS_DH,
    CRYPT_VERIFYCONTEXT);
}

void
provider::destroy(
  void
  )
{
  CryptReleaseContext(
    _dh_provider_handle,
    0);
  _dh_provider_handle = 0;

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

HCRYPTPROV
provider::get_dh_handle(
  void
  )
{
  return _dh_provider_handle;
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

ptr<dh1024>
provider::create_dh1024(
  void
  )
{
  return new dh1024(this);
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
