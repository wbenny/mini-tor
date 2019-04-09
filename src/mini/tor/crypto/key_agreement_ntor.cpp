#include "key_agreement_ntor.h"

#include <mini/crypto/random.h>
#include <mini/crypto/base16.h>
#include <mini/crypto/rfc5869.h>
#include <mini/crypto/hmac_sha256.h>

namespace mini::tor {

//
// definition of few constants.
// note that strings should be casted to string_ref,
// so that trailing '\0' byte is omitted.
//
// static constexpr byte_buffer_ref server_str("Server");
// static constexpr byte_buffer_ref protoid   ("ntor-curve25519-sha256-1");
// static constexpr byte_buffer_ref t_mac     ("ntor-curve25519-sha256-1" ":mac");
// static constexpr byte_buffer_ref t_key     ("ntor-curve25519-sha256-1" ":key_extract");
// static constexpr byte_buffer_ref t_verify  ("ntor-curve25519-sha256-1" ":verify");
// static constexpr byte_buffer_ref m_expand  ("ntor-curve25519-sha256-1" ":key_expand");
//

static constexpr byte_type const_server[] = {
  'S', 'e', 'r', 'v', 'e', 'r'
};

static constexpr byte_type const_protoid[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1'
};

static constexpr byte_type const_t_mac[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'm', 'a', 'c'
};

static constexpr byte_type const_t_key[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'k', 'e', 'y', '_', 'e', 'x', 't', 'r', 'a', 'c', 't'
};

static constexpr byte_type const_t_verify[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'v', 'e', 'r', 'i', 'f', 'y'
};

static constexpr byte_type const_m_expand[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'k', 'e', 'y', '_', 'e', 'x', 'p', 'a', 'n', 'd'
};

key_agreement_ntor::key_agreement_ntor(
  onion_router* router
  )
  //
  // generate key-pair.
  //
  : key_agreement_ntor(router, crypto::curve25519::private_key::generate())
{

}

key_agreement_ntor::key_agreement_ntor(
  onion_router* router,
  crypto::curve25519::private_key&& private_key
  )
  //
  // generate key-pair.
  //
  : key_agreement(router)
  , _private_key(std::move(private_key))
{

}

byte_buffer_ref
key_agreement_ntor::get_public_key(
  void
  ) const
{
  return _private_key.get_public_key_buffer();
}

byte_buffer_ref
key_agreement_ntor::get_private_key(
  void
  ) const
{
  return _private_key.get_private_key_buffer();
}

byte_buffer
key_agreement_ntor::compute_shared_secret(
  const byte_buffer_ref handshake_data
  )
{
  auto other_public_key = handshake_data.slice(
    0,
    crypto::curve25519::public_key::key_size_in_bytes);

  auto verify = handshake_data.slice(
    crypto::curve25519::public_key::key_size_in_bytes,
    crypto::curve25519::public_key::key_size_in_bytes + crypto::hmac_sha256::hash_size_in_bytes);

  return compute_shared_secret(other_public_key, verify);
}

byte_buffer
key_agreement_ntor::compute_shared_secret(
  const byte_buffer_ref other_public_key,
  const byte_buffer_ref verification_data
  )
{
  //
  // 5.1.4. The "ntor" handshake
  //
  // In this section, define:
  //    H(x,t) as HMAC_SHA256 with message x and key t.
  //    H_LENGTH  = 32.
  //    ID_LENGTH = 20.
  //    G_LENGTH  = 32
  //    PROTOID   = "ntor-curve25519-sha256-1"
  //    t_mac     = PROTOID | ":mac"
  //    t_key     = PROTOID | ":key_extract"
  //    t_verify  = PROTOID | ":verify"
  //    MULT(a,b) = the multiplication of the curve25519 point 'a' by the
  //                scalar 'b'.
  //    G         = The preferred base point for curve25519 ([9])
  //    KEYGEN()  = The curve25519 key generation algorithm, returning
  //                a private/public keypair.
  //    m_expand  = PROTOID | ":key_expand"
  //    KEYID(A)  = A
  //
  // To perform the handshake, the client needs to know an identity key
  // digest for the server, and an ntor onion key (a curve25519 public
  // key) for that server. Call the ntor onion key "B".  The client
  // generates a temporary keypair:
  //     x,X = KEYGEN()
  // and generates a client-side handshake with contents:
  //     NODEID      Server identity digest  [ID_LENGTH bytes]
  //     KEYID       KEYID(B)                [H_LENGTH bytes]
  //     CLIENT_PK   X                       [G_LENGTH bytes]
  //
  // The server generates a keypair of y,Y = KEYGEN(), and uses its ntor
  // private key 'b' to compute:
  //
  //   secret_input = EXP(X,y) | EXP(X,b) | ID | B | X | Y | PROTOID
  //   KEY_SEED = H(secret_input, t_key)
  //   verify = H(secret_input, t_verify)
  //   auth_input = verify | ID | B | Y | X | PROTOID | "Server"
  //
  // The server's handshake reply is:
  //     SERVER_PK   Y                       [G_LENGTH bytes]
  //     AUTH        H(auth_input, t_mac)    [H_LENGTH bytes]
  //
  // The client then checks Y is in G^* [see NOTE below], and computes
  //
  //   secret_input = EXP(Y,x) | EXP(B,x) | ID | B | X | Y | PROTOID
  //   KEY_SEED = H(secret_input, t_key)
  //   verify = H(secret_input, t_verify)
  //   auth_input = verify | ID | B | Y | X | PROTOID | "Server"
  //
  // The client verifies that AUTH == H(auth_input, t_mac).
  //

  auto shared_key1 = _private_key.get_shared_secret(other_public_key);
  auto shared_key2 = _private_key.get_shared_secret(_onion_router->get_ntor_onion_key());

  byte_buffer secret_input = {
    shared_key1,
    shared_key2,
    _onion_router->get_identity_fingerprint(),
    _onion_router->get_ntor_onion_key(),
    _private_key.get_public_key_buffer(),
    other_public_key,
    const_protoid
  };

  auto verify = crypto::hmac_sha256::compute(const_t_verify, secret_input);

  byte_buffer auth_input = {
    verify,
    _onion_router->get_identity_fingerprint(),
    _onion_router->get_ntor_onion_key(),
    other_public_key,
    _private_key.get_public_key_buffer(),
    const_protoid,
    const_server
  };

  auto computed_verification_data = crypto::hmac_sha256::compute(const_t_mac, auth_input);

  if (verification_data.equals(computed_verification_data))
  {
    //
    // create key material.
    //
    return crypto::rfc5869<crypto::hmac_sha256>::derive_key(
      secret_input,
      const_m_expand,
      const_t_key,
      92);
  }

  return byte_buffer();
}

}
