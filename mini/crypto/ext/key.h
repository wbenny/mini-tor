#pragma once
#include "../base/key.h"
#include <mini/common.h>

namespace mini::crypto::ext {

//
// MS CryptoAPI key.
//

class key
  : public base::key
{
  public:
    ~key(
      void
      ) override
    {

    }

    void
    destroy(
      void
      ) override
    {

    }

  protected:
    key(
      void
      ) = default;

    void
    swap(
      key& /*other*/
      )
    {

    }
};

}
