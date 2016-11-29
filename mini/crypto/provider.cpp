#include "provider.h"

namespace mini::crypto {

provider provider_factory;

provider::provider(
  void
  )
{
  init();
}

provider::~provider(
  void
  )
{
  destroy();
}

HCRYPTPROV
provider::get_rsa_aes_handle(
  void
  )
{
  return _provider_rsa_aes_handle;
}

HCRYPTPROV
provider::get_dh_handle(
  void
  )
{
  return _provider_dh_handle;
}


void
provider::init(
  void
  )
{
  CryptAcquireContext(
    &_provider_rsa_aes_handle,
    NULL,
    MS_ENH_RSA_AES_PROV,
    PROV_RSA_AES,
    CRYPT_VERIFYCONTEXT);

  CryptAcquireContext(
    &_provider_dh_handle,
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
  if (_provider_dh_handle)
  {
    CryptReleaseContext(
      _provider_dh_handle,
      0);
    _provider_dh_handle = 0;
  }

  if (_provider_rsa_aes_handle)
  {
    CryptReleaseContext(
      _provider_rsa_aes_handle,
      0);
    _provider_rsa_aes_handle = 0;
  }
}

}
