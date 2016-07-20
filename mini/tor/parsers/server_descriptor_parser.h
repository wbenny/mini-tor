#pragma once
#include <mini/crypto/base64.h>
#include <mini/collections/list.h>
#include <mini/string.h>
#include <mini/string_hash.h>
#include <mini/stack_buffer.h>

#include <cstdint>

namespace mini::tor {

struct server_descriptor_parser
{
  enum class document_location
  {
    control_word,

    onion_key,
    onion_key_content,

    signing_key,
    signing_key_content,
  };

  enum control_word_type
  {
    control_word_onion_key,
    control_word_signing_key,

    control_word_key_begin,
    control_word_key_end,
  };

  using control_word_list = stack_buffer<string_hash, 4>;
  static constexpr control_word_list control_words = {
    "onion-key",
    "signing-key",
    "-----BEGIN RSA PUBLIC KEY-----",
    "-----END RSA PUBLIC KEY-----",
  };

  void
  parse(
    onion_router* router,
    const string& descriptor
    )
  {
    string_collection lines = descriptor.split("\n");
    document_location current_location = document_location::control_word;
    string current_key;

    for (auto&& line : lines)
    {
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
    }
  }
};

}
