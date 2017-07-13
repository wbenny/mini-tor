#include <cstdint>

namespace mini::crypto::ext::detail {

int
curve25519_donna(
  uint8_t *mypublic,
  const uint8_t *secret,
  const uint8_t *basepoint
  );

}

