#pragma once
#include <mini/byte_buffer.h>
#include <mini/stack_buffer.h>

#include <windows.h>
#include <wincrypt.h>

#include "key.h"

namespace mini::crypto {

enum class aes_mode : DWORD
{
  cbc = CRYPT_MODE_CBC,
  ecb = CRYPT_MODE_ECB,
  ofb = CRYPT_MODE_OFB,
  cfb = CRYPT_MODE_CFB,
  cts = CRYPT_MODE_CTS,
};

template <
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
    static constexpr size_type key_size          = KEY_SIZE;
    static constexpr size_type key_size_in_bytes = KEY_SIZE / 8;

    struct blob
    {
      using provider_type = provider_rsa_aes_tag;

      BLOBHEADER header;
      DWORD      size;
      BYTE       key[key_size_in_bytes];
    };

    aes_key(
      void
      ) = default;

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
    import(
      const byte_buffer_ref key
      );

  private:
    static constexpr ALG_ID _key_alg  = key_size == 128
                       ? CALG_AES_128 : key_size == 192
                       ? CALG_AES_192 : key_size == 256
                       ? CALG_AES_256 : 0;
};

template <
  size_type KEY_SIZE
>
class aes
{
  MINI_MAKE_NONCOPYABLE(aes);

  public:
    static constexpr size_type key_size          = KEY_SIZE;
    static constexpr size_type key_size_in_bytes = KEY_SIZE / 8;

    using key  = aes_key<key_size>;
    using mode = aes_mode;

    aes(
      void
      ) = default;

    aes(
      mode mode,
      key&& k
      );

    ~aes(
      void
      ) = default;

    void
    init(
      mode mode,
      key&& k
      );

    void
    update_inplace(
      mutable_byte_buffer_ref buffer,
      bool do_final
      );

    void
    update(
      const byte_buffer_ref input,
      mutable_byte_buffer_ref output,
      bool do_final
      );

    byte_buffer
    update(
      const byte_buffer_ref input,
      bool do_final
      );

  private:
    key _key;
    mode _mode;
};

//
// AES-CTR
//

template <
  size_type KEY_SIZE
>
class aes_ctr
  : private aes<KEY_SIZE>
{
  MINI_MAKE_NONCOPYABLE(aes_ctr);

  using base_type = aes<KEY_SIZE>;

  public:
    static constexpr size_type key_size          = KEY_SIZE;
    static constexpr size_type key_size_in_bytes = KEY_SIZE / 8;

    using key  = typename aes<KEY_SIZE>::key;
    using mode = typename aes<KEY_SIZE>::mode;

    aes_ctr(
      void
      ) = default;

    aes_ctr(
      key&& k
      );

    ~aes_ctr(
      void
      ) = default;

    void
    init(
      key&& k
      );

    void
    update_inplace(
      mutable_byte_buffer_ref buffer
      );

    void
    update(
      const byte_buffer_ref input,
      mutable_byte_buffer_ref output
      );

    byte_buffer
    update(
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
