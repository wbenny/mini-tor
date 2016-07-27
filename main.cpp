#include <mini/logger.h>
#include <mini/console.h>
#include <mini/crypto/random.h>
#include <mini/io/stream_reader.h>
#include <mini/tor/circuit.h>
#include <mini/tor/consensus.h>
#include <mini/tor/tor_socket.h>
#include <mini/tor/tor_stream.h>

#define MINI_TOR_USE_CONSENSUS_CACHE 1

class tor_client
{
  public:
    tor_client(
      void
      )
    {

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

      auto random_router = routers[mini::crypto::random_device->get_random(routers.get_size())];

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
          "Connecting to: '%s' (%s:%u)",
          onion_router->get_name().get_buffer(),
          onion_router->get_ip_address().to_string().get_buffer(),
          onion_router->get_or_port());

        _socket.connect(onion_router);
        _circuit = _socket.create_circuit();

        mini_info("Connected...");
      }
      else
      {
        mini_info(
          "Extending to: '%s' (%s:%u)",
          onion_router->get_name().get_buffer(),
          onion_router->get_ip_address().to_string().get_buffer(),
          onion_router->get_or_port());

        _circuit->extend(onion_router);

        mini_info("Extended...");
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
        extend_to(onion_router_name);
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
        mini_info("Created...");
      }
      else
      {
        stream = _circuit->create_stream(host, port);
      }

      mini_info("Sending request...");
      mini::string req = "GET " + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";
      stream->write(req.get_buffer(), req.get_size());
      mini_info("Request sent...");

      mini_info("Receiving response...");
      mini::io::stream_reader sr(*stream);
      result = sr.read_string_to_end();
      mini_info("Response received...");

      delete stream;

      return result;
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

int
__cdecl main(
  int argc,
  char* argv[]
  )
{
  if (argc != 2)
  {
    mini::console::write("No parametr provided!\n");
    mini::console::write("Usage:\n");
    mini::console::write("  mini-tor [url]\n");
    mini::console::write("Example:\n");
    mini::console::write("  mini-tor \"http://duskgytldkxiuqc6.onion/fedpapers/federndx.htm\"\n");
    return -1;
  }

#if defined(_DEBUG)
  mini::log.set_level(mini::logger::level::info);
#endif

  size_t hops = 2;

  mini_info("Fetching consensus...");
  tor_client tor;
  mini_info("Consensus fetched...");

  tor.extend_to_random(
    mini::tor::onion_router::status_flag::fast |
    mini::tor::onion_router::status_flag::running |
    mini::tor::onion_router::status_flag::valid,
    { 80, 443 });

  hops--;
  while (hops-- > 1)
  {
    tor.extend_to_random(
      mini::tor::onion_router::status_flag::fast |
      mini::tor::onion_router::status_flag::running |
      mini::tor::onion_router::status_flag::valid);
  }

  tor.extend_to_random(
    mini::tor::onion_router::status_flag::fast |
    mini::tor::onion_router::status_flag::running |
    mini::tor::onion_router::status_flag::valid |
    mini::tor::onion_router::status_flag::exit);

  mini::string content = tor.http_get(0 ? "http://duskgytldkxiuqc6.onion/fedpapers/federndx.htm" : argv[1]);
  mini::console::write("%s", content.get_buffer());

  mini_info("");
  mini_info("-----------------------------");
  mini_info("content size: %u bytes", content.get_size());
  mini_info("-----------------------------");

  return 0;
}