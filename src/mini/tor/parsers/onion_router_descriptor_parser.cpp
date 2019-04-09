#include "onion_router_descriptor_parser.h"

#include <mini/crypto/base64.h>

namespace mini::tor {

constexpr onion_router_descriptor_parser::control_word_list onion_router_descriptor_parser::control_words;

void
onion_router_descriptor_parser::parse(
  onion_router* router,
  const string_ref descriptor
  )
{
  string_collection lines = static_cast<string>(descriptor).split("\n");
  document_location current_location = document_location::control_word;
  string current_key;

  for (auto&& line : lines)
  {
    string_collection splitted_line = line.split(" ");
    string_hash control_word_hash = splitted_line[0];

    //
    // onion-key
    //
    if (line == control_words[control_word_onion_key])
    {
      current_location = document_location::onion_key;
      continue;
    }
    //
    // signing-key
    //
    else if (line == control_words[control_word_signing_key])
    {
      current_location = document_location::signing_key;
      continue;
    }
    //
    // -----BEGIN RSA PUBLIC KEY-----
    //
    else if (line == control_words[control_word_key_begin])
    {
      if (current_location == document_location::onion_key)
      {
        current_location = document_location::onion_key_content;
      }
      else if (current_location == document_location::signing_key)
      {
        current_location = document_location::signing_key_content;
      }
      continue;
    }
    //
    // -----END RSA PUBLIC KEY-----
    //
    else if (line == control_words[control_word_key_end])
    {
      if (current_location == document_location::onion_key_content)
      {
        router->set_onion_key(crypto::base64::decode(current_key));
      }
      else if (current_location == document_location::signing_key_content)
      {
        router->set_signing_key(crypto::base64::decode(current_key));
      }
      current_location = document_location::control_word;
      current_key.clear();
    }
    else if (current_location == document_location::onion_key_content ||
             current_location == document_location::signing_key_content)
    {
      current_key += line;
    }
    else if (control_word_hash == control_words[control_word_ntor_onion_key])
    {
      router->set_ntor_onion_key(crypto::base64::decode(splitted_line[1]));
    }
  }
}

}
