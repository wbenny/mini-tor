#include "key_agreement_tap.h"

#include <mini/crypto/random.h>
#include <mini/crypto/sha1.h>

namespace mini::tor {

//
// tor-spec.txt
// 0.2.
//
//  DH_SEC_LEN -- the number of bytes used in a Diffie-Hellman private key (x).
//
// tor-spec.txt
// 0.3.
//
// As an optimization, implementations SHOULD choose DH private keys (x) of
// 320 bits.
//
// static constexpr size_type DH_SEC_LEN = 40;

//
//  DH_LEN -- the number of bytes used to represent a member of the
//    Diffie-Hellman group.
//
// static constexpr size_type DH_LEN = 128;

//
// For Diffie-Hellman, we use a generator (g) of 2.
//
static const byte_type DH_G[] = {
  2
};

//
// For the modulus (p), we
// use the 1024-bit safe prime from rfc2409 section 6.2 whose hex
// representation is:
//
static const byte_type DH_P[] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc9, 0x0f, 0xda, 0xa2, 0x21, 0x68, 0xc2, 0x34,
  0xc4, 0xc6, 0x62, 0x8b, 0x80, 0xdc, 0x1c, 0xd1, 0x29, 0x02, 0x4e, 0x08, 0x8a, 0x67, 0xcc, 0x74,
  0x02, 0x0b, 0xbe, 0xa6, 0x3b, 0x13, 0x9b, 0x22, 0x51, 0x4a, 0x08, 0x79, 0x8e, 0x34, 0x04, 0xdd,
  0xef, 0x95, 0x19, 0xb3, 0xcd, 0x3a, 0x43, 0x1b, 0x30, 0x2b, 0x0a, 0x6d, 0xf2, 0x5f, 0x14, 0x37,
  0x4f, 0xe1, 0x35, 0x6d, 0x6d, 0x51, 0xc2, 0x45, 0xe4, 0x85, 0xb5, 0x76, 0x62, 0x5e, 0x7e, 0xc6,
  0xf4, 0x4c, 0x42, 0xe9, 0xa6, 0x37, 0xed, 0x6b, 0x0b, 0xff, 0x5c, 0xb6, 0xf4, 0x06, 0xb7, 0xed,
  0xee, 0x38, 0x6b, 0xfb, 0x5a, 0x89, 0x9f, 0xa5, 0xae, 0x9f, 0x24, 0x11, 0x7c, 0x4b, 0x1f, 0xe6,
  0x49, 0x28, 0x66, 0x51, 0xec, 0xe6, 0x53, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

//
// 5.2.1. KDF-TOR
//
//   This key derivation function is used by the TAP and CREATE_FAST
//   handshakes, and in the current hidden service protocol. It shouldn't
//   be used for new functionality.
//
//   If the TAP handshake is used to extend a circuit, both parties
//   base their key material on K0=g^xy, represented as a big-endian unsigned
//   integer.
//
//   If CREATE_FAST is used, both parties base their key material on
//   K0=X|Y.
//
//   From the base key material K0, they compute KEY_LEN*2+HASH_LEN*3 bytes of
//   derivative key data as
//       K = H(K0 | [00]) | H(K0 | [01]) | H(K0 | [02]) | ...
//
//   The first HASH_LEN bytes of K form KH; the next HASH_LEN form the forward
//   digest Df; the next HASH_LEN 41-60 form the backward digest Db; the next
//   KEY_LEN 61-76 form Kf, and the final KEY_LEN form Kb.  Excess bytes from K
//   are discarded.
//
//   KH is used in the handshake response to demonstrate knowledge of the
//   computed shared key. Df is used to seed the integrity-checking hash
//   for the stream of data going from the OP to the OR, and Db seeds the
//   integrity-checking hash for the data stream from the OR to the OP. Kf
//   is used to encrypt the stream of data going from the OP to the OR, and
//   Kb is used to encrypt the stream of data going from the OR to the OP.
//

static byte_buffer
derive_keys(
  const byte_buffer_ref secret
  )
{
  byte_buffer key_material;
  key_material.reserve(100);

  byte_buffer hashdata(secret);
  hashdata.resize(hashdata.get_size() + 1); // increase by 1 byte
                                            // (make room for counter)

  for (byte_type i = 0; i < 5; i++)
  {
    hashdata[secret.get_size()] = i;
    key_material.add_many(crypto::sha1::compute(hashdata));
  }

  return key_material;
}

key_agreement_tap::key_agreement_tap(
  onion_router* router
  )
  //
  // generate key-pair.
  //
  : key_agreement_tap(router, dh1024::private_key::generate(DH_G, DH_P))
{

}

key_agreement_tap::key_agreement_tap(
  onion_router* router,
  dh1024::private_key&& private_key
  )
  //
  // generate key-pair.
  //
  : key_agreement(router)
  , _private_key(std::move(private_key))
{

}

byte_buffer_ref
key_agreement_tap::get_public_key(
  void
  ) const
{
  return _private_key.get_y();
}

byte_buffer_ref
key_agreement_tap::get_private_key(
  void
  ) const
{
  return _private_key.get_exponent();
}

byte_buffer
key_agreement_tap::compute_shared_secret(
  const byte_buffer_ref handshake_data
  )
{
  return compute_shared_secret(
    handshake_data.slice(0, dh1024::key_size_in_bytes),
    handshake_data.slice(dh1024::key_size_in_bytes, dh1024::key_size_in_bytes + crypto::sha1::hash_size_in_bytes));
}

byte_buffer
key_agreement_tap::compute_shared_secret(
  const byte_buffer_ref other_public_key,
  const byte_buffer_ref verification_data
  )
{
  //
  // 5.1.3. The "TAP" handshake
  //
  // This handshake uses Diffie-Hellman in Z_p and RSA to compute a set of
  // shared keys which the client knows are shared only with a particular
  // server, and the server knows are shared with whomever sent the
  // original handshake (or with nobody at all).  It's not very fast and
  // not very good.  (See Goldberg's "On the Security of the Tor
  // Authentication Protocol".)
  //
  // Define TAP_C_HANDSHAKE_LEN as DH_LEN+KEY_LEN+PK_PAD_LEN.
  // Define TAP_S_HANDSHAKE_LEN as DH_LEN+HASH_LEN.
  //
  // The payload for a CREATE cell is an 'onion skin', which consists of
  // the first step of the DH handshake data (also known as g^x).  This
  // value is hybrid-encrypted (see 0.3) to the server's onion key, giving
  // a client handshake of:
  //
  //     PK-encrypted:
  //       Padding                       [PK_PAD_LEN bytes]
  //       Symmetric key                 [KEY_LEN bytes]
  //       First part of g^x             [PK_ENC_LEN-PK_PAD_LEN-KEY_LEN bytes]
  //     Symmetrically encrypted:
  //       Second part of g^x            [DH_LEN-(PK_ENC_LEN-PK_PAD_LEN-KEY_LEN)
  //                                         bytes]
  //
  // The payload for a CREATED cell, or the relay payload for an
  // EXTENDED cell, contains:
  //       DH data (g^y)                 [DH_LEN bytes]
  //       Derivative key data (KH)      [HASH_LEN bytes]   <see 5.2 below>
  //
  // Once the handshake between the OP and an OR is completed, both can
  // now calculate g^xy with ordinary DH.  Before computing g^xy, both parties
  // MUST verify that the received g^x or g^y value is not degenerate;
  // that is, it must be strictly greater than 1 and strictly less than p-1
  // where p is the DH modulus.  Implementations MUST NOT complete a handshake
  // with degenerate keys.  Implementations MUST NOT discard other "weak"
  // g^x values.
  //
  // (Discarding degenerate keys is critical for security; if bad keys
  // are not discarded, an attacker can substitute the OR's CREATED
  // cell's g^y with 0 or 1, thus creating a known g^xy and impersonating
  // the OR. Discarding other keys may allow attacks to learn bits of
  // the private key.)
  //
  // Once both parties have g^xy, they derive their shared circuit keys
  // and 'derivative key data' value via the KDF-TOR function in 5.2.1.
  //

  mini_assert(verification_data.get_size() == crypto::sha1::hash_size_in_bytes);

  auto shared_secret = _private_key.get_shared_secret(other_public_key);
  auto derived = derive_keys(shared_secret);

  //
  // first 20 bytes of the derived key is the verification checksum.
  // rest of it is the key material.
  //
  auto computed_verification_data = derived.slice(0, crypto::sha1::hash_size_in_bytes);
  auto key_material               = derived.slice(crypto::sha1::hash_size_in_bytes);

  if (computed_verification_data.equals(verification_data))
  {
    return key_material;
  }

  return byte_buffer();
}

}
