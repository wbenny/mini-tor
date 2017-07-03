#include "aes.h"

//
// inspired by:
//   - https://github.com/t6x/reaver-wps-fork-t6x/blob/master/src/crypto/crypto_cryptoapi.c
//   - https://modexp.wordpress.com/2016/03/10/windows-ctr-mode-with-crypto-api/
//

namespace mini::crypto {

//
// AES
// key.
//

template <
  size_type KEY_SIZE
>
aes_key<KEY_SIZE>::aes_key(
  const byte_buffer_ref key
  )
{
  import(key);
}

template <
  size_type KEY_SIZE
>
aes_key<KEY_SIZE>::aes_key(
  aes_key&& other
  )
{
  this->swap(other);
}

template <
  size_type KEY_SIZE
>
aes_key<KEY_SIZE>&
aes_key<KEY_SIZE>::operator=(
  aes_key&& other
  )
{
  this->swap(other);
  return *this;
}

template <
  size_type KEY_SIZE
>
void
aes_key<KEY_SIZE>::import(
  const byte_buffer_ref key
  )
{
  //
  // destroy previous key.
  //
  this->destroy();

  blob key_blob = {
    { PLAINTEXTKEYBLOB, CUR_BLOB_VERSION, 0, _key_alg },
    key_size_in_bytes
  };

  memory::copy(key_blob.key, key.get_buffer(), key_blob.size);

  import_from_blob(key_blob);
}

//
// AES
//

template <
  size_type KEY_SIZE
>
aes<KEY_SIZE>::aes(
  typename aes::mode mode,
  typename aes::key&& aes_key
  )
{
  init(mode, std::move(aes_key));
}

template <
  size_type KEY_SIZE
>
void
aes<KEY_SIZE>::init(
  typename aes::mode mode,
  typename aes::key&& aes_key
  )
{
  _key  = std::move(aes_key);
  _mode = mode;

  CryptSetKeyParam(
    _key.get_handle(),
    KP_MODE,
    (BYTE*)&mode,
    0);
}

template <
  size_type KEY_SIZE
>
void
aes<KEY_SIZE>::update_inplace(
  mutable_byte_buffer_ref output,
  bool do_final
  )
{
  DWORD crypted_size = key_size_in_bytes;
  CryptEncrypt(
    _key.get_handle(),
    0,
    do_final,
    0,
    output.get_buffer(),
    &crypted_size,
    key_size_in_bytes);
}

template <
  size_type KEY_SIZE
>
void
aes<KEY_SIZE>::update(
  const byte_buffer_ref input,
  mutable_byte_buffer_ref output,
  bool do_final
  )
{
  mini_assert(input.get_size() == output.get_size());

  output.copy_from(input);
  update_inplace(output, do_final);
}

template <
  size_type KEY_SIZE
>
byte_buffer
aes<KEY_SIZE>::update(
  const byte_buffer_ref input,
  bool do_final
  )
{
  byte_buffer result(input.get_size());
  update(input, result, do_final);

  return result;
}

//
// AES-CTR
//

template <
  size_type KEY_SIZE
>
aes_ctr<KEY_SIZE>::aes_ctr(
  key&& k
  )
{
  init(std::move(k));
}

template <
  size_type KEY_SIZE
>
void
aes_ctr<KEY_SIZE>::init(
  key&& k
  )
{
  memory::zero(_counter);
  memory::zero(_counter_out);

  _keystream_pointer = 0xFFFF;

  base_type::init(mode::ecb, std::move(k));
}

template <
  size_type KEY_SIZE
>
void
aes_ctr<KEY_SIZE>::update_inplace(
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
aes_ctr<KEY_SIZE>::update(
  const byte_buffer_ref input,
  mutable_byte_buffer_ref output
  )
{
  mini_assert(input.get_size() == output.get_size());

  output.copy_from(input);
  update_inplace(output);
}

template <
  size_type KEY_SIZE
>
byte_buffer
aes_ctr<KEY_SIZE>::update(
  const byte_buffer_ref input
  )
{
  byte_buffer result(input.get_size());
  update(input, result);

  return result;
}

template <
  size_type KEY_SIZE
>
byte_buffer
aes_ctr<KEY_SIZE>::crypt(
  key&& k,
  const byte_buffer_ref input
  )
{
  return aes_ctr<key_size>(std::move(k)).update(input);
}

template <
  size_type KEY_SIZE
>
byte_type
aes_ctr<KEY_SIZE>::next_keystream_byte(
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
aes_ctr<KEY_SIZE>::update_counter(
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
aes_ctr<KEY_SIZE>::encrypt_counter(
  void
  )
{
  base_type::update(_counter, _counter_out, false);
}

template <
  size_type KEY_SIZE
>
void
aes_ctr<KEY_SIZE>::increment_counter(
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
