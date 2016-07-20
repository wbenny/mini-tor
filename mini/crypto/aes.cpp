#include "aes.h"
#include "provider.h"

//
// inspired by:
//   - https://github.com/t6x/reaver-wps-fork-t6x/blob/master/src/crypto/crypto_cryptoapi.c
//

namespace mini::crypto {

aes::aes(
  provider* crypto_provider)
  : _provider(crypto_provider)
  , _key(0)
{

}

aes::~aes(
  void
  )
{
  destroy();
}

void
aes::init(
  aes::mode aes_mode,
  aes::key_size aes_key_size,
  const byte_buffer_ref key
  )
{
  _mode = aes_mode;
  _key_size = aes_key_size == key_size_128
    ? 16 : aes_key_size == key_size_192
    ? 24 : aes_key_size == key_size_256
    ? 32 : 0;

  struct ms_aes_key
  {
    BLOBHEADER header;
    DWORD      size;
    BYTE       key[32];
  };

  memset(_counter, 0, 32);
  memset(_counter_out, 0, 32);
  _keystream_pointer = 0xFFFF;

  //
  // initialize WinCrypt AES-128 key.
  //
  ms_aes_key key_blob;
  key_blob.header.bType = PLAINTEXTKEYBLOB;
  key_blob.header.bVersion = CUR_BLOB_VERSION;
  key_blob.header.reserved = 0;
  key_blob.header.aiKeyAlg = aes_key_size;
  key_blob.size = _key_size;

  memcpy(key_blob.key, key.get_buffer(), key_blob.size);

  BOOL result;
  result = CryptImportKey(
    _provider->get_handle(),
    (BYTE *)&key_blob,
    sizeof(key_blob),
    0,
    0,
    &_key);

  _mode = aes_mode;

  //
  // WinCrypt cannot do CTR mode, we have to do it manually.
  //
  DWORD mode = aes_mode == mode_ctr
    ? CRYPT_MODE_ECB
    : aes_mode;

  result = CryptSetKeyParam(
    _key,
    KP_MODE,
    (BYTE *)&mode,
    0);
}

void
aes::destroy(
  void
  )
{
  CryptDestroyKey(_key);
}

void
aes::update(
  const byte_buffer_ref input,
  mutable_byte_buffer_ref output,
  bool do_final
  )
{
  if (_mode == mode_ctr)
  {
    for (uint8_t byte : input)
    {
      *output++ = byte ^ next_keystream_byte();
    }
  }
  else
  {
    do_update(input, output, do_final);
  }
}

byte_buffer
aes::update(
  const byte_buffer_ref input,
  bool do_final
  )
{
  byte_buffer result(input.get_size());
  update(input, result, do_final);

  return result;
}

byte_buffer
aes::crypt(
  aes::mode aes_mode,
  aes::key_size aes_key_size,
  const byte_buffer_ref key,
  const byte_buffer_ref input
  )
{
  ptr<aes> aes = provider_factory.create_aes();
  aes->init(aes_mode, aes_key_size, key);
  return aes->update(input, (aes_mode == mode::mode_ctr || aes_mode == mode::mode_ecb) ? false : true);
}

uint8_t
aes::next_keystream_byte(
  void
  )
{
  if (_keystream_pointer >= _key_size)
  {
    update_counter();
  }

  return _counter_out[_keystream_pointer++];
}

void
aes::update_counter(
  void
  )
{
  encrypt_counter();
  increment_counter();
  _keystream_pointer = 0;
}

void
aes::encrypt_counter(
  void
  )
{
  do_update(_counter, _counter_out, false);
}

void
aes::increment_counter(
  void
  )
{
  int carry = 1;
  for (int i = _key_size - 1; i >= 0; i--)
  {
    int x = _counter[i] + carry;

    if (x > 0xff)
    {
      carry = 1;
    }
    else
    {
      carry = 0;
    }

    _counter[i] = (uint8_t)x;
  }
}


void
aes::do_update(
  const byte_buffer_ref input,
  mutable_byte_buffer_ref output,
  bool do_final
  )
{
  memcpy(output.get_buffer(), input.get_buffer(), _key_size);

  DWORD crypted_size = _key_size;
  BOOL result;
  result = CryptEncrypt(
    _key,
    0,
    do_final,
    0,
    output.get_buffer(),
    &crypted_size,
    _key_size);
}

}
