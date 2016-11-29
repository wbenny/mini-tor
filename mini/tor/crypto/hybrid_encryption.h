#pragma once
#include <mini/byte_buffer.h>

namespace mini::tor::hybrid_encryption {

//
// tor-spec.txt
// 0.3.
//
static constexpr size_type KEY_LEN = 16;
static constexpr size_type PK_ENC_LEN = 128;
static constexpr size_type PK_PAD_LEN = 42;

static constexpr size_type PK_DATA_LEN = PK_ENC_LEN - PK_PAD_LEN;
static constexpr size_type PK_DATA_LEN_WITH_KEY = PK_DATA_LEN - KEY_LEN;

//
// Encrypt the entire contents of the byte array "data" with the given "TorPublicKey"
// according to the "hybrid encryption" scheme described in the main Tor specification(tor-spec.txt).
//
byte_buffer
encrypt(
  const byte_buffer_ref data,
  const byte_buffer_ref public_key
  );

}
