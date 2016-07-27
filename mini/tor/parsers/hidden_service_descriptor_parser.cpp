#pragma once
#include "hidden_service_descriptor_parser.h"

namespace mini::tor {

void
hidden_service_descriptor_parser::parse(
  consensus& consensus,
  const string_ref descriptor
  )
{
  string_collection lines = static_cast<string>(descriptor).split("\n");
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

}
