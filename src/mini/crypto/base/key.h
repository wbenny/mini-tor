#pragma once

namespace mini::crypto::base {

class key
{
  public:
    virtual ~key(
      void
      ) = default;

    virtual void
    destroy(
      void
      ) = 0;

  protected:
    key(
      void
      ) = default;
};

}
