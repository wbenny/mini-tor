#pragma once
#include <mini/common.h>

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto::capi {

class provider
{
  MINI_MAKE_NONCOPYABLE(provider);

  public:
    provider(
      void
      );

    ~provider(
      void
      );

    HCRYPTPROV
    get_rsa_aes_handle(
      void
      );

    HCRYPTPROV
    get_dh_handle(
      void
      );

  private:
    void
    init(
      void
      );

    void
    destroy(
      void
      );

    HCRYPTPROV _provider_rsa_aes_handle = 0;
    HCRYPTPROV _provider_dh_handle      = 0;
};

extern provider provider_factory;

}
