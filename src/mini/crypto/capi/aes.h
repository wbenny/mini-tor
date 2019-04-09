#pragma once
#include "key.h"
#include "../common.h"

#include <mini/byte_buffer.h>
#include <mini/stack_buffer.h>

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto::capi {

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
class aes_key
  : public key
{
  MINI_MAKE_NONCOPYABLE(aes_key);

  static_assert(
    KEY_SIZE == 128 ||
    KEY_SIZE == 192 ||
    KEY_SIZE == 256,
    "valid AES key sizes are: 128, 192, 256");

  public:
    static constexpr size_type   key_size          = KEY_SIZE;
    static constexpr size_type   key_size_in_bytes = KEY_SIZE / 8;
    static constexpr cipher_mode mode              = AES_MODE;

    aes_key(
      void
      );

    aes_key(
      const byte_buffer_ref key
      );

    aes_key(
      aes_key&& other
      );

    aes_key&
    operator=(
      aes_key&& other
      );

    void
    swap(
      aes_key& other
      );

    //
    // import.
    //

    void
    import(
      const byte_buffer_ref key
      );

    byte_buffer_ref
    get_iv(
      void
      ) const;

    void
    set_iv(
      const byte_buffer_ref iv
      );

  public:
    struct blob
    {
      struct provider_type
      {
        static constexpr auto get_handle = &provider::get_rsa_aes_handle;
      };

      BLOBHEADER header;
      DWORD      size;
      BYTE       key[key_size_in_bytes];
    };

  private:
    static constexpr ALG_ID _key_alg  = key_size == 128
                       ? CALG_AES_128 : key_size == 192
                       ? CALG_AES_192 : key_size == 256
                       ? CALG_AES_256 : 0;

    blob _blob;
    byte_type _iv[key_size_in_bytes];
};

//
// AES
//

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
class aes
{
  MINI_MAKE_NONCOPYABLE(aes);

  public:
    static constexpr size_type   key_size          = KEY_SIZE;
    static constexpr size_type   key_size_in_bytes = KEY_SIZE / 8;
    static constexpr cipher_mode mode              = AES_MODE;

    using key = aes_key<AES_MODE, KEY_SIZE>;

    aes(
      void
      ) = default;

    aes(
      key&& k
      );

    ~aes(
      void
      ) = default;

    void
    init(
      key&& k
      );

    void
    encrypt_inplace(
      mutable_byte_buffer_ref buffer
      );

    void
    encrypt(
      const byte_buffer_ref input,
      mutable_byte_buffer_ref output
      );

    byte_buffer
    encrypt(
      const byte_buffer_ref input
      );

    void
    decrypt_inplace(
      mutable_byte_buffer_ref buffer
      );

    void
    decrypt(
      const byte_buffer_ref input,
      mutable_byte_buffer_ref output
      );

    byte_buffer
    decrypt(
      const byte_buffer_ref input
      );

    static byte_buffer
    encrypt(
      key&& k,
      const byte_buffer_ref input
      );

    static byte_buffer
    decrypt(
      key&& k,
      const byte_buffer_ref input
      );

  private:
    key _key;
};

//
// AES-CTR
//

template <
  size_type KEY_SIZE
>
class aes<cipher_mode::ctr, KEY_SIZE>
  : private aes<cipher_mode::ecb, KEY_SIZE>
{
  MINI_MAKE_NONCOPYABLE(aes);

  using base_type = aes<cipher_mode::ecb, KEY_SIZE>;

  public:
    static constexpr size_type   key_size          = KEY_SIZE;
    static constexpr size_type   key_size_in_bytes = KEY_SIZE / 8;
    static constexpr cipher_mode mode              = cipher_mode::ctr;

    using key = aes_key<cipher_mode::ctr, KEY_SIZE>;

    aes(
      void
      ) = default;

    aes(
      key&& k
      );

    ~aes(
      void
      ) = default;

    void
    init(
      key&& k
      );

    void
    encrypt_inplace(
      mutable_byte_buffer_ref buffer
      );

    void
    encrypt(
      const byte_buffer_ref input,
      mutable_byte_buffer_ref output
      );

    byte_buffer
    encrypt(
      const byte_buffer_ref input
      );

    //
    // encrypt/decrypt in ctr mode are the same.
    //

    void
    decrypt_inplace(
      mutable_byte_buffer_ref buffer
      );

    void
    decrypt(
      const byte_buffer_ref input,
      mutable_byte_buffer_ref output
      );

    byte_buffer
    decrypt(
      const byte_buffer_ref input
      );

    static byte_buffer
    crypt(
      key&& k,
      const byte_buffer_ref input
      );

  private:
    byte_type
    next_keystream_byte(
      void
      );

    void
    update_counter(
      void
      );

    void
    encrypt_counter(
      void
      );

    void
    increment_counter(
      void
      );

    stack_byte_buffer<key_size_in_bytes> _counter;
    stack_byte_buffer<key_size_in_bytes> _counter_out;
    size_type _keystream_pointer;
};

}

#include "aes.inl"
