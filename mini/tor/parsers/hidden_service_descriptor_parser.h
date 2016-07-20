#pragma once
#include <mini/crypto/base64.h>
#include <mini/collections/list.h>
#include <mini/collections/pair_list.h>
#include <mini/string.h>
#include <mini/string_hash.h>
#include <mini/stack_buffer.h>
#include <mini/net/ip_address.h>
#include <mini/tor/consensus.h>
#include <mini/tor/parsers/introduction_point_parser.h>

#include <cstdint>

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
    const string& descriptor
    )
  {
    string_collection lines = descriptor.split("\n");
    document_location current_location = document_location::control_word;
    string current_message;

    for (auto&& line : lines)
    {
      //
      // introduction-points
      //
      if (line == control_words[control_word_introduction_points])
      {
        current_location = document_location::introduction_points;
        continue;
      }
      //
      // -----BEGIN MESSAGE-----
      //
      else if (line == control_words[control_word_message_begin])
      {
        current_location = document_location::introduction_points_content;
        continue;
      }
      //
      // -----END MESSAGE-----
      //
      else if (line == control_words[control_word_message_end])
      {
        current_location = document_location::control_word;
        break;
      }
      else if (current_location == document_location::introduction_points_content)
      {
        current_message += line;
      }
    }

    //
    // introduction points are base64 encoded.
    //
    byte_buffer introduction_point_descriptor = crypto::base64::decode(current_message);
    string introduction_point_descriptor_string;
    introduction_point_descriptor_string.assign(
      (char*)introduction_point_descriptor.get_buffer(),
      introduction_point_descriptor.get_size());

    //
    // parse the introduction point descriptor.
    //
    introduction_point_parser parser;
    parser.parse(consensus, introduction_point_descriptor_string);

    introduction_point_list = std::move(parser.introduction_point_list);
  }
};


}
