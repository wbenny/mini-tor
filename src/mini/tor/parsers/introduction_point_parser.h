#pragma once
#include <mini/string.h>
#include <mini/string_hash.h>
#include <mini/stack_buffer.h>
#include <mini/tor/consensus.h>
#include <mini/tor/onion_router.h>

namespace mini::tor {

struct introduction_point_parser
{
  onion_router_list introduction_point_list;

  enum class document_location
  {
    control_word,

    service_key,
    service_key_content,
  };

  enum control_word_type
  {
    control_word_introduction_point,
    control_word_service_key,

    control_word_key_begin,
    control_word_key_end,
  };

  using control_word_list = stack_buffer<string_hash, 4>;
  static constexpr control_word_list control_words = { {
    "introduction-point",
    "service-key",
    "-----BEGIN RSA PUBLIC KEY-----",
    "-----END RSA PUBLIC KEY-----",
  } };

  void
  parse(
    consensus& consensus,
    const string_ref descriptor
    );
};


}
