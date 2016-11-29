#pragma once
#include "provider.h"

#include <mini/common.h>

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto {

class key
{
  public:
    virtual ~key(
      void
      )
    {
      destroy();
    }

    virtual void
    destroy(
      void
      )
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
      )
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
      import_from_blob(
        key_blob,
        typename BLOB_TYPE::provider_type());
    }

    template <
      typename BLOB_TYPE
    >
    void
    export_to_blob(
      DWORD blob_type,
      BLOB_TYPE& key_blob
      ) const
    {
      BYTE* key_blob_ptr = reinterpret_cast<BYTE*>(&key_blob);
      DWORD key_blob_size = static_cast<DWORD>(sizeof(key_blob));

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
    //
    // import_from_blob specializations.
    //

    template <
      typename BLOB_TYPE
    >
    void
    import_from_blob(
      const BLOB_TYPE& key_blob,
      provider_rsa_aes_tag
      )
    {
      import_from_blob(
        reinterpret_cast<const BYTE*>(&key_blob),
        static_cast<DWORD>(sizeof(key_blob)),
        provider_factory.get_rsa_aes_handle()
      );
    }

    template <
      typename BLOB_TYPE
    >
    void
    import_from_blob(
      const BLOB_TYPE& key_blob,
      provider_dh_tag
      )
    {
      import_from_blob(
        reinterpret_cast<const BYTE*>(&key_blob),
        static_cast<DWORD>(sizeof(key_blob)),
        provider_factory.get_dh_handle()
        );
    }

    void
    import_from_blob(
      const BYTE* key_blob,
      DWORD key_blob_size,
      HCRYPTPROV provider_handle
      )
    {
      //
      // destroy previous key.
      //
      this->destroy();

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
