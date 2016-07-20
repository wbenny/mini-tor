#pragma once
#include <mini/common.h>
#include <mini/ptr.h>

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto {

class aes;
class sha1;
class rsa;
class random;

class provider
{
  public:
    MINI_MAKE_NONCOPYABLE(provider);

    provider(
      void
      );

    ~provider(
      void
      );

    void
    init(
      void
      );

    void
    destroy(
      void
      );

    HCRYPTPROV
    get_handle(
      void
      );

    ptr<aes>
    create_aes(
      void
      );

    ptr<sha1>
    create_sha1(
      void
      );

    ptr<rsa>
    create_rsa(
      void
      );

    ptr<random>
    create_random(
      void
      );

  private:
    HCRYPTPROV _provider_handle;
};

extern provider provider_factory;

}
