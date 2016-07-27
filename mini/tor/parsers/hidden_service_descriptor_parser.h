#pragma once
#include <mini/string.h>
#include <mini/string_hash.h>
#include <mini/stack_buffer.h>
#include <mini/collections/list.h>
#include <mini/collections/pair_list.h>
#include <mini/crypto/base64.h>
#include <mini/net/ip_address.h>
#include <mini/tor/consensus.h>
#include <mini/tor/parsers/introduction_point_parser.h>

namespace mini::tor {

struct hidden_service_descriptor_parser
{
  onion_router_list introduction_point_list;

  enum class document_location
  {
    control_word,

    introduction_points,
    introduction_points_content,
  };

  enum control_word_type
  {
    control_word_introduction_points,

    control_word_message_begin,
    control_word_message_end,
  };

  using control_word_list = stack_buffer<string_hash, 3>;
  static constexpr control_word_list control_words = {
    "introduction-points",
    "-----BEGIN MESSAGE-----",
    "-----END MESSAGE-----",
  };

  void
  parse(
    consensus& consensus,
    const string_ref descriptor
    );
};


}
