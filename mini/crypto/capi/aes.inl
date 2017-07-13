#include "aes.h"

//
// inspired by:
//   - https://github.com/t6x/reaver-wps-fork-t6x/blob/master/src/crypto/crypto_cryptoapi.c
//   - https://modexp.wordpress.com/2016/03/10/windows-ctr-mode-with-crypto-api/
//

namespace mini::crypto::capi {

namespace detail {

  //
  // map each value from cipher_mode to its
  // corresponding CryptoAPI definition.
  //
  static constexpr DWORD cipher_mode_map[] = {
    CRYPT_MODE_CBC,
    CRYPT_MODE_ECB,
    CRYPT_MODE_OFB,
    CRYPT_MODE_CFB,
    CRYPT_MODE_CTS,
  };

}

//
// AES
// key.
//

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
aes_key<AES_MODE, KEY_SIZE>::aes_key(
  void
  )
  : _blob({
    { PLAINTEXTKEYBLOB, CUR_BLOB_VERSION, 0, _key_alg },
      key_size_in_bytes
  })
  , _iv()
{

}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
aes_key<AES_MODE, KEY_SIZE>::aes_key(
  const byte_buffer_ref key
  )
  : aes_key<AES_MODE, KEY_SIZE>()
{
  import(key);
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
aes_key<AES_MODE, KEY_SIZE>::aes_key(
  aes_key&& other
  )
{
  swap(other);
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
aes_key<AES_MODE, KEY_SIZE>&
aes_key<AES_MODE, KEY_SIZE>::operator=(
  aes_key&& other
  )
{
  swap(other);
  return *this;
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
void
aes_key<AES_MODE, KEY_SIZE>::swap(
  aes_key& other
  )
{
  key::swap(other);
  mini::swap(_blob, other._blob);
  mini::swap(_iv, other._iv);
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
void
aes_key<AES_MODE, KEY_SIZE>::import(
  const byte_buffer_ref key
  )
{
  memory::copy(_blob.key, &key[0], _blob.size);

  import_from_blob(_blob);
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
byte_buffer_ref
aes_key<AES_MODE, KEY_SIZE>::get_iv(
  void
  ) const
{
  return _iv;
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
void
aes_key<AES_MODE, KEY_SIZE>::set_iv(
  const byte_buffer_ref iv
  )
{
  iv.copy_to(_iv);
}

//
// AES
//

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
aes<AES_MODE, KEY_SIZE>::aes(
  typename aes::key&& aes_key
  )
{
  //
  // we do not initialize _iv in ctor - it will be copied from the key.
  //
  init(std::move(aes_key));
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
void
aes<AES_MODE, KEY_SIZE>::init(
  typename aes::key&& aes_key
  )
{
  _key  = std::move(aes_key);

  CryptSetKeyParam(
    _key.get_handle(),
    KP_MODE,
    (BYTE*)&detail::cipher_mode_map[static_cast<int>(AES_MODE)],
    0);

  //
  // this condition should be optimized out completely
  // when AES_MODE == cipher_mode::ecb.
  //
  if (AES_MODE != cipher_mode::ecb)
  {
    CryptSetKeyParam(
      _key.get_handle(),
      KP_IV,
      _key.get_iv().get_buffer(),
      0);
  }
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
void
aes<AES_MODE, KEY_SIZE>::encrypt_inplace(
  mutable_byte_buffer_ref buffer
  )
{
  DWORD buffer_size = key_size_in_bytes;
  CryptEncrypt(
    _key.get_handle(),
    0,
    FALSE, // do_final
    0,
    buffer.get_buffer(),
    &buffer_size,
    key_size_in_bytes);
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
void
aes<AES_MODE, KEY_SIZE>::encrypt(
  const byte_buffer_ref input,
  mutable_byte_buffer_ref output
  )
{
  mini_assert(input.get_size() == output.get_size());

  output.copy_from(input);
  encrypt_inplace(output);
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
byte_buffer
aes<AES_MODE, KEY_SIZE>::encrypt(
  const byte_buffer_ref input
  )
{
  byte_buffer result(input.get_size());
  encrypt(input, result);

  return result;
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
void
aes<AES_MODE, KEY_SIZE>::decrypt_inplace(
  mutable_byte_buffer_ref buffer
  )
{
  DWORD buffer_size = key_size_in_bytes;
  CryptDecrypt(
    _key.get_handle(),
    0,
    FALSE, // do_final
    0,
    buffer.get_buffer(),
    &buffer_size,
    key_size_in_bytes);
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
void
aes<AES_MODE, KEY_SIZE>::decrypt(
  const byte_buffer_ref input,
  mutable_byte_buffer_ref output
  )
{
  mini_assert(input.get_size() == output.get_size());

  output.copy_from(input);
  decrypt_inplace(output);
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
byte_buffer
aes<AES_MODE, KEY_SIZE>::decrypt(
  const byte_buffer_ref input
  )
{
  byte_buffer result(input.get_size());
  decrypt(input, result);

  return result;
}

//
// AES-CTR
//

template <
  size_type KEY_SIZE
>
aes<cipher_mode::ctr, KEY_SIZE>::aes(
  key&& k
  )
{
  init(std::move(k));
}

template <
  size_type KEY_SIZE
>
void
aes<cipher_mode::ctr, KEY_SIZE>::init(
  key&& k
  )
{
  memory::zero(_counter);
  memory::zero(_counter_out);

  _keystream_pointer = 0xFFFF;

  base_type::init(std::move(reinterpret_cast<aes_key<cipher_mode::ecb, KEY_SIZE>&>(k)));
}

template <
  size_type KEY_SIZE
>
void
aes<cipher_mode::ctr, KEY_SIZE>::encrypt_inplace(
  mutable_byte_buffer_ref buffer
  )
{
  for (byte_type& byte : buffer)
  {
    byte ^= next_keystream_byte();
  }
}

template <
  size_type KEY_SIZE
>
void
aes<cipher_mode::ctr, KEY_SIZE>::encrypt(
  const byte_buffer_ref input,
  mutable_byte_buffer_ref output
  )
{
  mini_assert(input.get_size() == output.get_size());

  output.copy_from(input);
  encrypt_inplace(output);
}

template <
  size_type KEY_SIZE
>
byte_buffer
aes<cipher_mode::ctr, KEY_SIZE>::encrypt(
  const byte_buffer_ref input
  )
{
  byte_buffer result(input.get_size());
  encrypt(input, result);

  return result;
}

template <
  size_type KEY_SIZE
>
void
aes<cipher_mode::ctr, KEY_SIZE>::decrypt_inplace(
  mutable_byte_buffer_ref buffer
  )
{
  encrypt_inplace(buffer);
}

template <
  size_type KEY_SIZE
>
void
aes<cipher_mode::ctr, KEY_SIZE>::decrypt(
  const byte_buffer_ref input,
  mutable_byte_buffer_ref output
  )
{
  encrypt(input, output);
}

template <
  size_type KEY_SIZE
>
byte_buffer
aes<cipher_mode::ctr, KEY_SIZE>::decrypt(
  const byte_buffer_ref input
  )
{
  return encrypt(input);
}

template <
  size_type KEY_SIZE
>
byte_buffer
aes<cipher_mode::ctr, KEY_SIZE>::crypt(
  key&& k,
  const byte_buffer_ref input
  )
{
  return aes<cipher_mode::ctr, KEY_SIZE>(std::move(k)).encrypt(input);
}

template <
  size_type KEY_SIZE
>
byte_type
aes<cipher_mode::ctr, KEY_SIZE>::next_keystream_byte(
  void
  )
{
  if (_keystream_pointer >= key_size_in_bytes)
  {
    update_counter();
  }

  return _counter_out[_keystream_pointer++];
}

template <
  size_type KEY_SIZE
>
void
aes<cipher_mode::ctr, KEY_SIZE>::update_counter(
  void
  )
{
  encrypt_counter();
  increment_counter();
  _keystream_pointer = 0;
}

template <
  size_type KEY_SIZE
>
void
aes<cipher_mode::ctr, KEY_SIZE>::encrypt_counter(
  void
  )
{
  base_type::encrypt(_counter, _counter_out);
}

template <
  size_type KEY_SIZE
>
void
aes<cipher_mode::ctr, KEY_SIZE>::increment_counter(
  void
  )
{
  for (int i = static_cast<int>(key_size_in_bytes) - 1; i >= 0; i--)
  {
    if (++_counter[i])
    {
      break;
    }
  }
}

}
