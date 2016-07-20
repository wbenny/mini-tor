#pragma once
#include "big_integer.h"

#include <mini/common.h>

namespace mini::crypto {

static BI_CTX* bigint_ctx = nullptr;

void
big_integer::global_init(
  void
  )
{
  if (bigint_ctx == nullptr)
  {
    bigint_ctx = bi_initialize();
  }
}

void
big_integer::global_destroy(
  void
  )
{
  bi_terminate(bigint_ctx);
  bigint_ctx = nullptr;
}

//
// constructors.
//

big_integer::big_integer(
  void
  )
  : _impl(nullptr)
{
  global_init();
}

big_integer::big_integer(
  const byte_buffer_ref input
  )
  : big_integer()
{
  from_bytes(input);
}

big_integer::big_integer(
  const big_integer& other
  )
  : big_integer()
{
  _impl = bi_clone(bigint_ctx, other._impl);
}


big_integer::big_integer(
  big_integer&& other
  )
  : big_integer()
{
  swap(_impl, other._impl);
}

big_integer::big_integer(
  ::bigint* impl
  )
  : _impl(impl)
{

}

//
// destructor.
//

big_integer::~big_integer()
{
  if (_impl != nullptr)
  {
    bi_free(bigint_ctx, _impl);
  }
}

//
// assign operator.
//

big_integer& big_integer::operator=(
  const big_integer& other
  )
{
  _impl = bi_clone(bigint_ctx, other._impl);

  return *this;
}

big_integer& big_integer::operator=(
  big_integer&& other
  )
{
  swap(_impl, other._impl);

  return *this;
}

big_integer& big_integer::operator=(
  const byte_buffer_ref input
  )
{
  from_bytes(input);

  return *this;
}

//
// conversion methods.
//

void
big_integer::from_bytes(
  const byte_buffer_ref input
  )
{
  _impl = bi_import(bigint_ctx, input.get_buffer(), (int)input.get_size());
}

byte_buffer
big_integer::to_bytes(
  void
  ) const
{
  byte_buffer result(_impl->size * COMP_BYTE_SIZE);
  bi_export(bigint_ctx, _impl, result.get_buffer(), (int)result.get_size());

  return result;
}

//
// operations.
//

big_integer
big_integer::mod_pow(
  const big_integer& modulus,
  const big_integer& exponent
  ) const
{
  ::bigint* biR;
  ::bigint* tmp_biR;

  BI_CTX* tmp_ctx = bi_initialize();
  {
    bi_set_mod(
      tmp_ctx,
      bi_clone(tmp_ctx, (::bigint*)modulus._impl),
      BIGINT_M_OFFSET);

    tmp_biR = bi_mod_power(
      tmp_ctx,
      bi_clone(tmp_ctx, _impl),
      bi_clone(tmp_ctx, (::bigint*)exponent._impl));

    biR = bi_clone(bigint_ctx, tmp_biR);

    bi_free(tmp_ctx, tmp_biR);
    bi_free_mod(tmp_ctx, BIGINT_M_OFFSET);
  }
  bi_terminate(tmp_ctx);

  return big_integer(biR);
}

big_integer::operator byte_buffer(
  void
  ) const
{
  return to_bytes();
}

}
