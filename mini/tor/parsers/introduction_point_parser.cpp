#pragma once
#include "introduction_point_parser.h"

#include <mini/crypto/base16.h>
#include <mini/crypto/base32.h>
#include <mini/crypto/base64.h>

namespace mini::tor {

void
introduction_point_parser::parse(
  consensus& consensus,
  const string& descriptor
  )
{
  string_collection lines = descriptor.split("\n");
  document_location current_location = document_location::control_word;
  onion_router* current_router = nullptr;
  string current_key;

  for (auto&& line : lines)
  {
    string_collection splitted_line = line.split(" ");
    string_hash control_word_hash = splitted_line[0];

    //
    // introduction-point
    //
    if (control_word_hash == control_words[control_word_introduction_point])
    {
      string identity_fingerprint = crypto::base16::encode(crypto::base32::decode(splitted_line[1]));
      current_router = consensus.get_onion_router_by_identity_fingerprint(identity_fingerprint);
      continue;
    }
    //
    // service-key
    //
    else if (control_word_hash == control_words[control_word_service_key])
    {
      current_location = document_location::service_key;
      continue;
    }
    //
    // -----BEGIN RSA PUBLIC KEY-----
    //
    else if (line == control_words[control_word_key_begin] && current_location == document_location::service_key)
    {
      current_location = document_location::service_key_content;
      continue;
    }
    //
    // -----END RSA PUBLIC KEY-----
    //
    else if (line == control_words[control_word_key_end] && current_location == document_location::service_key_content)
    {
      if (current_router)
      {
        current_router->set_service_key(crypto::base64::decode(current_key));
        introduction_point_list.add(current_router);
      }

      current_location = document_location::control_word;
      current_key.clear();
    }
    else if (current_location == document_location::service_key_content)
    {
      current_key += line;
    }
  }
}


}
