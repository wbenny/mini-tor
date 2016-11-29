#include "consensus_parser.h"

namespace mini::tor {

constexpr consensus_parser::preamble_control_word_list          consensus_parser::preamble_control_words;
constexpr consensus_parser::router_status_entry                 consensus_parser::router_status_entry_chars;
constexpr consensus_parser::router_status_flags_type            consensus_parser::router_status_flags;
constexpr consensus_parser::directory_footer_control_word_list  consensus_parser::directory_footer_control_words;

onion_router::status_flags
consensus_parser::string_to_status_flags(
  const string_collection& splitted
  )
{
  onion_router::status_flags result = onion_router::status_flag::none;

  for (auto&& flag_string : splitted)
  {
    auto index = router_status_flags.index_of(string_hash(flag_string));

    if (index != router_status_flags_type::not_found)
    {
      onion_router::status_flag flag = onion_router::status_flag(1 << index);
      result |= flag;
    }
  }

  return result;
}

void
consensus_parser::parse(
  consensus& consensus,
  const string_ref content,
  bool reject_invalid
  )
{
  string_collection lines = static_cast<string>(content).split("\n");
  document_location current_location = document_location::preamble;
  onion_router* current_router = nullptr;

  for (auto&& line : lines)
  {
    auto splitted_line = line.split(" ");

    //
    // move the location if we are at the router status entries.
    //
    if (splitted_line[0].get_size() == 1 && splitted_line[0][0] == router_status_entry_chars[router_status_entry_r])
    {
      current_location = document_location::router_status_entry;
    }
    else if (splitted_line[0] == directory_footer_control_words[directory_footer])
    {
      current_location = document_location::directory_footer;
    }

    switch (current_location)
    {
      case document_location::preamble:
        {
          if (splitted_line[0] == preamble_control_words[preamble_type::preamble_valid_until])
          {
            consensus._valid_until.parse(splitted_line[1] + " " + splitted_line[2]);

            if (reject_invalid && consensus._valid_until < time::now())
            {
              return;
            }
          }
        }
        break;

      case document_location::router_status_entry:
        {
          //
          // check if the control word has at least one letter.
          //
          if (splitted_line[0].get_size() < 1)
          {
            break;
          }

          mini_assert(splitted_line[0].get_size() == 1);

          switch (splitted_line[0][0])
          {
            case router_status_entry_chars[router_status_entry_r]:
              {
                //
                // router.
                //
                if (splitted_line.get_size() < router_status_entry_r_item_count)
                {
                  //
                  // next line.
                  //
                  continue;
                }

                string identity_fingerprint
                  = crypto::base16::encode(
                      crypto::base64::decode(
                        splitted_line[router_status_entry_r_identity]));

                current_router = new onion_router(
                  consensus,
                  splitted_line[router_status_entry_r_nickname],
                  splitted_line[router_status_entry_r_ip],
                  static_cast<uint16_t>(splitted_line[router_status_entry_r_or_port].to_int()),
                  static_cast<uint16_t>(splitted_line[router_status_entry_r_dir_port].to_int()),
                  identity_fingerprint);

                consensus._onion_router_map.insert(identity_fingerprint, current_router);
              }
              break;

            case router_status_entry_chars[router_status_entry_s]:
              {
                //
                // flags.
                //
                if (current_router != nullptr)
                {
                  current_router->set_flags(string_to_status_flags(line.split(" ")));
                }
              }
              break;
          }
        }
        break;

      case document_location::directory_footer:
        //
        // ignore directory footer.
        //
        goto consensus_parsed;

      //
    } // switch (current_location)
      //

    //
  } // for (auto&& line : consensus_lines)
    //

consensus_parsed:
  ;
}

}
