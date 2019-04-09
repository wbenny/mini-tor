#pragma once
#include "provider.h"
#include "../base/key.h"

#include <mini/common.h>

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto::capi {

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
      destroy();
    }

    void
    destroy(
      void
      ) override
    {
      if (_key_handle)
      {
        CryptDestroyKey(_key_handle);
        _key_handle = 0;
      }
    }

    HCRYPTKEY
    get_handle(
      void
      ) const
    {
      return _key_handle;
    }

    operator bool(
      void
      ) const
    {
      return _key_handle != 0;
    }

  protected:
    key(
      void
      ) = default;

    void
    swap(
      key& other
      )
    {
      mini::swap(_key_handle, other._key_handle);
    }

    template <
      typename BLOB_TYPE
    >
    void
    import_from_blob(
      const BLOB_TYPE& key_blob
      )
    {
      //
      // destroy previous key.
      //
      destroy();

      //
      // ayy lmao.
      //
      auto key_blob_ptr    = reinterpret_cast<const BYTE*>(&key_blob);
      auto key_blob_size   = static_cast<DWORD>(sizeof(key_blob));
      auto provider_handle = (provider_factory.*BLOB_TYPE::provider_type::get_handle)();

      import_from_blob(
        key_blob_ptr,
        key_blob_size,
        provider_handle);
    }

    template <
      typename BLOB_TYPE
    >
    void
    export_to_blob(
      BLOB_TYPE& key_blob,
      DWORD blob_type
      ) const
    {
      auto key_blob_ptr  = reinterpret_cast<BYTE*>(&key_blob);
      auto key_blob_size = static_cast<DWORD>(sizeof(key_blob));

      CryptExportKey(
        _key_handle,
        0,
        blob_type,
        0,
        key_blob_ptr,
        &key_blob_size);
    }

    HCRYPTKEY _key_handle = 0;

  private:
    void
    import_from_blob(
      const BYTE* key_blob,
      DWORD key_blob_size,
      HCRYPTPROV provider_handle
      )
    {
      CryptImportKey(
        provider_handle,
        key_blob,
        key_blob_size,
        0,
        CRYPT_EXPORTABLE,
        &_key_handle);
    }
};

}
