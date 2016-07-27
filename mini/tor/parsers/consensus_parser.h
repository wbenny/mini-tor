#pragma once
#include <mini/time.h>
#include <mini/string.h>
#include <mini/string_hash.h>
#include <mini/stack_buffer.h>
#include <mini/crypto/base16.h>
#include <mini/crypto/base64.h>
#include <mini/tor/consensus.h>

namespace mini::tor {

struct consensus_parser
{
  //
  // dir-spec.txt
  // 3.4.1.
  //
  // Status documents contain a preamble, an authority section, a list of
  // router status entries, and one or more footer signature, in that order.
  //

  enum class document_location
  {
    preamble,
    router_status_entry,
    directory_footer,
  };

  //
  // preamble.
  //
  enum preamble_type
  {
    preamble_valid_until,
  };

  using preamble_control_word_list = stack_buffer<string_hash, 1>;
  static constexpr preamble_control_word_list preamble_control_words = {
    "valid-until",
  };

  //
  // router status entry.
  //
  enum router_status_entry_type
  {
    router_status_entry_r,
    router_status_entry_a,
    router_status_entry_s,
    router_status_entry_v,
    router_status_entry_w,
    router_status_entry_p,
  };

  using router_status_entry = stack_buffer<char, 7>;
  static constexpr router_status_entry router_status_entry_chars = {
    'r',
    'a',
    's',
    'v',
    'w',
    'p',
  };

  enum router_status_entry_r_type
  {
    //
    // start counting from 1,
    // because there is the "r" control word
    // on the index 0.
    //
    router_status_entry_r_nickname = 1,
    router_status_entry_r_identity,
    router_status_entry_r_digest,
    router_status_entry_r_publication_date,
    router_status_entry_r_publication_time,
    router_status_entry_r_ip,
    router_status_entry_r_or_port,
    router_status_entry_r_dir_port,

    //
    // router_status_entry_r_item_count = 9
    //
    router_status_entry_r_item_count,
  };

  enum router_status_entry_s_type
  {
    router_status_entry_s_none            = 0x0000,
    router_status_entry_s_authority       = 0x0001,
    router_status_entry_s_bad_exit        = 0x0002,
    router_status_entry_s_exit            = 0x0004,
    router_status_entry_s_fast            = 0x0008,
    router_status_entry_s_guard           = 0x0010,
    router_status_entry_s_hsdir           = 0x0020,
    router_status_entry_s_named           = 0x0040,
    router_status_entry_s_no_ed_consensus = 0x0080,
    router_status_entry_s_stable          = 0x0100,
    router_status_entry_s_running         = 0x0200,
    router_status_entry_s_unnamed         = 0x0400,
    router_status_entry_s_valid           = 0x0800,
    router_status_entry_s_v2dir           = 0x1000,
  };

  using router_status_flags_type = stack_buffer<string_hash, 13>;
  static constexpr router_status_flags_type router_status_flags = {
    "Authority",
    "BadExit",
    "Exit",
    "Fast",
    "Guard",
    "HSDir",
    "Named",
    "NoEdConsensus",
    "Stable",
    "Running",
    "Unnamed",
    "Valid",
    "V2Dir",
  };

  //
  // directory footer.
  //
  enum directory_footer_type
  {
    directory_footer,
  };

  using directory_footer_control_word_list = stack_buffer<string_hash, 1>;
  static constexpr directory_footer_control_word_list directory_footer_control_words = {
    "directory-footer",
  };

  //
  // implementation.
  //

  onion_router::status_flags
  string_to_status_flags(
    const collections::list<string>& splitted
    );

  void
  parse(
    consensus& consensus,
    const string& content,
    bool reject_invalid = true
    );
};

}
