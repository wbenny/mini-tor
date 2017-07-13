#pragma once
#include "provider.h"
#include "../base/key.h"

#include <mini/common.h>

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto::cng {

//
// used to distinguish symmetric and asymmetric encryption algorithms.
//
struct provider_key_symmetric_tag {};
struct provider_key_asymmetric_tag {};


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
        BCryptDestroyKey(_key_handle);
        _key_handle = 0;
      }
    }

    BCRYPT_KEY_HANDLE
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
      return _key_handle != nullptr;
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
      auto key_blob_type   = BLOB_TYPE::provider_type::blob_type;
      auto provider_handle = (provider_factory.*BLOB_TYPE::provider_type::get_handle)();

      using key_type       = typename BLOB_TYPE::provider_type::key_type;

      import_from_blob(
        key_blob_ptr,
        key_blob_size,
        key_blob_type,
        provider_handle,
        key_type());
    }

    template <
      typename BLOB_TYPE
    >
    void
    export_to_blob(
      BLOB_TYPE& key_blob,
      LPCWSTR blob_type
      ) const
    {
      auto key_blob_ptr  = reinterpret_cast<BYTE*>(&key_blob);
      auto key_blob_size = static_cast<DWORD>(sizeof(key_blob));

      BCryptExportKey(
        _key_handle,
        nullptr,
        blob_type,
        key_blob_ptr,
        key_blob_size,
        &key_blob_size,
        0);
    }

    BCRYPT_KEY_HANDLE _key_handle = 0;

  private:
    //
    // import_from_blob specializations.
    //

    void
    import_from_blob(
      const BYTE* key_blob,
      DWORD key_blob_size,
      LPCWSTR key_blob_type,
      BCRYPT_ALG_HANDLE provider_handle,
      provider_key_asymmetric_tag
      )
    {
      BCryptImportKeyPair(
        provider_handle,
        nullptr,
        key_blob_type,
        &_key_handle,
        (PUCHAR)key_blob,
        key_blob_size,
        0);
    }

    void
    import_from_blob(
      const BYTE* key_blob,
      DWORD key_blob_size,
      LPCWSTR key_blob_type,
      BCRYPT_ALG_HANDLE provider_handle,
      provider_key_symmetric_tag
      )
    {
      BCryptImportKey(
        provider_handle,
        nullptr,
        key_blob_type,
        &_key_handle,
        nullptr,
        0,
        (PUCHAR)key_blob,
        key_blob_size,
        0);
    }
};

}
