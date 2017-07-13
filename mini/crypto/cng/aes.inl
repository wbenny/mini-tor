#include "aes.h"

namespace mini::crypto::cng {

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
    { BCRYPT_KEY_DATA_BLOB_MAGIC, BCRYPT_KEY_DATA_BLOB_VERSION1, key_size_in_bytes }
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
  memory::copy(_blob.key, &key[0], key_size_in_bytes);

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
  _key = std::move(aes_key);
  _key.get_iv().copy_to(_iv);
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
  buffer.copy_from(encrypt(buffer));
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

  ULONG output_size;
  BCryptEncrypt(
    _key.get_handle(),
    const_cast<PUCHAR>(input.get_buffer()),
    static_cast<ULONG>(input.get_size()),
    nullptr,
    AES_MODE != cipher_mode::ecb ? _iv : nullptr,
    AES_MODE != cipher_mode::ecb ? sizeof(_iv) : 0,
    static_cast<PUCHAR>(output.get_buffer()),
    static_cast<ULONG>(output.get_size()),
    &output_size,
    0);
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
  buffer.copy_from(decrypt(buffer));
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

  ULONG output_size;
  BCryptDecrypt(
    _key.get_handle(),
    const_cast<PUCHAR>(input.get_buffer()),
    static_cast<ULONG>(input.get_size()),
    AES_MODE != cipher_mode::ecb ? _iv : nullptr,
    AES_MODE != cipher_mode::ecb ? sizeof(_iv) : 0,
    key_size_in_bytes,
    static_cast<PUCHAR>(output.get_buffer()),
    static_cast<ULONG>(output.get_size()),
    &output_size,
    0);
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

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
byte_buffer
aes<AES_MODE, KEY_SIZE>::encrypt(
  key&& k,
  const byte_buffer_ref input
  )
{
  return aes<AES_MODE, KEY_SIZE>(std::move(k)).encrypt(input);
}

template <
  cipher_mode AES_MODE,
  size_type KEY_SIZE
>
byte_buffer
aes<AES_MODE, KEY_SIZE>::decrypt(
  key&& k,
  const byte_buffer_ref input
  )
{
  return aes<AES_MODE, KEY_SIZE>(std::move(k)).decrypt(input);
}

//
// AES-CTR
//

template <
  size_type KEY_SIZE
>
aes<cipher_mode::ctr, KEY_SIZE>::aes(
  typename aes::key&& k
  )
{
  init(std::move(k));
}

template <
  size_type KEY_SIZE
>
void
aes<cipher_mode::ctr, KEY_SIZE>::init(
  typename aes::key&& k
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
aes<cipher_mode::ctr, KEY_SIZE>::encrypt(
  key&& k,
  const byte_buffer_ref input
  )
{
  return crypt(std::move(k), input);
}

template <
  size_type KEY_SIZE
>
byte_buffer
aes<cipher_mode::ctr, KEY_SIZE>::decrypt(
  key&& k,
  const byte_buffer_ref input
  )
{
  return crypt(std::move(k), input);
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
