#include <mini/logger.h>
#include <mini/console.h>
#include <mini/crypto/random.h>
#include <mini/io/stream_reader.h>
#include <mini/io/file.h>
#include <mini/tor/circuit.h>
#include <mini/tor/consensus.h>
#include <mini/tor/tor_socket.h>
#include <mini/tor/tor_stream.h>
#include <mini/net/http.h>

#define MINI_TOR_USE_CONSENSUS_CACHE 1

class tor_client
{
  public:
    tor_client(
      void
      )
    {
      _consensus.set_allowed_dir_ports({ 80, 443 });
    }

    ~tor_client(
      void
      )
    {
      delete _circuit;
    }

    void
    extend_to_random(
      mini::tor::onion_router::status_flags flags,
      mini::collections::list<uint16_t> or_ports = {}
      )
    {
      auto routers = _consensus.get_onion_routers_by_criteria({
        {}, or_ports, _forbidden_onion_routers, flags
      });

      auto random_router = routers[mini::crypto::random_device.get_random(routers.get_size())];

      if (random_router)
      {
        _forbidden_onion_routers.add(random_router);
        extend_to(random_router);
      }
    }

    void
    extend_to(
      mini::tor::onion_router* onion_router
      )
    {
      if (_circuit == nullptr)
      {
        mini_info(
          "Connecting to node #%u: '%s' (%s:%u)",
          get_hop_count() + 1,
          onion_router->get_name().get_buffer(),
          onion_router->get_ip_address().to_string().get_buffer(),
          onion_router->get_or_port());

        _socket.connect(onion_router);

        if (_socket.is_connected())
        {
          _circuit = _socket.create_circuit();

          if (get_hop_count() == 1)
          {
            mini_info("Connected...");
          }
          else
          {
            mini_error("Error while creating circuit!");
          }
        }
        else
        {
          mini_error("Error while connecting!");
        }
      }
      else
      {
        mini_info(
          "Extending to node #%u: '%s' (%s:%u)",
          get_hop_count() + 1,
          onion_router->get_name().get_buffer(),
          onion_router->get_ip_address().to_string().get_buffer(),
          onion_router->get_or_port());

        auto previous_hop_count = get_hop_count();

        _circuit->extend(onion_router);

        if (get_hop_count() == (previous_hop_count + 1))
        {
          mini_info("Extended...");
        }
        else
        {
          delete _circuit;
          _circuit = nullptr;

          mini_warning("Error when extending!");
        }
      }
    }

    void
    extend_to(
      const mini::string_ref onion_router_name
      )
    {
      mini::tor::onion_router* router = _consensus.get_onion_router_by_name(onion_router_name);

      if (router)
      {
        extend_to(router);
      }
    }

    mini::string
    http_get(
      const mini::string_ref url
      )
    {
      mini::tor::tor_stream* stream = nullptr;
      mini::string result;

      mini::string url_string = url;

      mini_info("Accessing '%s'", url_string.get_buffer());
      if (url_string.starts_with("http://"))
      {
        url_string = url_string.substring(7);
      }

      if (url_string.contains("/") == false)
      {
        url_string += "/";
      }

      mini::string_collection url_parts = url_string.split("/", 1);
      mini::string host = url_parts[0];
      mini::string path = url_parts[1];
      uint16_t port = 80;

      if (host.ends_with(".onion"))
      {
        mini::string onion = host.substring(0, host.get_size() - 6);

        mini_info("Creating onion stream...");
        stream = _circuit->create_onion_stream(onion, port);
      }
      else
      {
        mini_info("Creating stream...");
        stream = _circuit->create_stream(host, port);
      }

      if (stream)
      {
        mini_info("Created...");
      }
      else
      {
        mini_error("Error while creating the onion stream");
        return mini::string();
      }

      mini_info("Sending request...");
      result = mini::net::http::client::get(host, port, path, *stream);
      mini_info("Response received...");

      delete stream;

      return result;
    }

    mini::size_type
    get_hop_count(
      void
      )
    {
      return _circuit
        ? _circuit->get_circuit_node_list().get_size()
        : 0;
    }

  private:
    mini::tor::consensus _consensus
#if defined (MINI_TOR_USE_CONSENSUS_CACHE)
      = mini::tor::consensus("cached-consensus")
#endif
      ;

    mini::tor::tor_socket _socket;
    mini::tor::circuit* _circuit = nullptr;
    mini::collections::list<mini::tor::onion_router*> _forbidden_onion_routers;
};

int __cdecl
main(
  int argc,
  char* argv[]
  )
{
  if (argc < 2)
  {
    mini::console::write("No parameter provided!\n");
    mini::console::write("Usage:\n");
    mini::console::write("  mini-tor [-v] [-vv] [-vvv] [url]\n");
    mini::console::write("Example:\n");
    mini::console::write("  mini-tor \"http://duskgytldkxiuqc6.onion/fedpapers/federndx.htm\"\n");
    return -1;
  }

  mini::log.set_level(mini::logger::level::off);

  //
  // parse arguments.
  //
  int arg_index = 1;
  if (mini::string_ref(argv[arg_index]).equals("-v"))
  {
    mini::log.set_level(mini::logger::level::warning);
    arg_index += 1;

    if (arg_index == argc)
    {
      return -1;
    }
  }

  if (mini::string_ref(argv[arg_index]).equals("-vv"))
  {
    mini::log.set_level(mini::logger::level::info);
    arg_index += 1;

    if (arg_index == argc)
    {
      return -1;
    }
  }

  if (mini::string_ref(argv[arg_index]).equals("-vvv"))
  {
    mini::log.set_level(mini::logger::level::debug);
    arg_index += 1;

    if (arg_index == argc)
    {
      return -1;
    }
  }

#if defined(_DEBUG)
  mini::log.set_level(mini::logger::level::info);
#endif

  //
  // fetch the page.
  //
  static constexpr mini::size_type hops = 2;
  static_assert(hops >= 2, "There must be at least 2 hops in the circuit");
  static_assert(hops <= 9, "There must be at most 9 hops in the circuit");

  mini_info("Fetching consensus...");
  tor_client tor;
  mini_info("Consensus fetched...");

connect_again:
  while (tor.get_hop_count() < hops)
  {
    //
    // first hop.
    //
    if (tor.get_hop_count() == 0)
    {
      tor.extend_to_random(
        mini::tor::onion_router::status_flag::fast |
        mini::tor::onion_router::status_flag::running |
        mini::tor::onion_router::status_flag::valid,
        { 80, 443 });
    }

    //
    // last hop (exit node).
    //
    else if (tor.get_hop_count() == (hops - 1))
    {
      tor.extend_to_random(
        mini::tor::onion_router::status_flag::fast |
        mini::tor::onion_router::status_flag::running |
        mini::tor::onion_router::status_flag::valid |
        mini::tor::onion_router::status_flag::exit);
    }

    //
    // middle hops.
    //
    else
    {
      tor.extend_to_random(
        mini::tor::onion_router::status_flag::fast |
        mini::tor::onion_router::status_flag::running |
        mini::tor::onion_router::status_flag::valid);
    }
  }

  mini::string content = tor.http_get(0 ? "http://duskgytldkxiuqc6.onion/fedpapers/federndx.htm" : argv[arg_index]);
  if (content.is_empty())
  {
    mini_info("Trying to build new circuit...");
    goto connect_again;
  }

  //mini::io::file::write_from_string("out.txt", content);

  mini::console::write("%s", content.get_buffer());

  mini_info("");
  mini_info("-----------------------------");
  mini_info("content size: %u bytes", content.get_size());
  mini_info("-----------------------------");


  return 0;
}

// */
