#pragma once
#include <mini/common.h>
#include <mini/byte_buffer.h>

#include <windows.h>
#include <wincrypt.h>

namespace mini::crypto {

class aes
{
  public:
    MINI_MAKE_NONCOPYABLE(aes);

    enum mode
    {
      mode_cbc = CRYPT_MODE_CBC,
      mode_ecb = CRYPT_MODE_ECB,
      mode_ofb = CRYPT_MODE_OFB,
      mode_cfb = CRYPT_MODE_CFB,
      mode_cts = CRYPT_MODE_CTS,
      mode_ctr = 1337,
    };

    enum key_size
    {
      key_size_128 = CALG_AES_128,
      key_size_192 = CALG_AES_192,
      key_size_256 = CALG_AES_256,
    };

    ~aes(
      void
      );

    void
    init(
      aes::mode aes_mode,
      aes::key_size aes_key_size,
      const byte_buffer_ref key
      );

    void
    destroy(
      void
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

    static byte_buffer
    crypt(
      aes::mode aes_mode,
      aes::key_size aes_key_size,
      const byte_buffer_ref key,
      const byte_buffer_ref input
      );

  private:
    friend class provider;

    aes(
      provider* crypto_provider
      );

    uint8_t
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

    void
    do_update(
      const byte_buffer_ref input,
      mutable_byte_buffer_ref output,
      bool do_final
      );

    provider* _provider;
    HCRYPTKEY _key;
    mode _mode;
    DWORD _key_size;

    uint8_t _counter[32];
    uint8_t _counter_out[32];
    size_t _keystream_pointer;
};

}
