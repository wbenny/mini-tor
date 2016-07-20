#include <winsock2.h>
#include <mini/common.h>
#include <mini/stack_buffer.h>
#include <mini/threading/thread.h>
#include <mini/threading/thread_function.h>
#include <mini/threading/mutex.h>
#include <mini/function.h>
#include <mini/console.h>
#include <mini/crypto/big_integer.h>
#include <mini/tor/consensus.h>
#include <mini/tor/circuit.h>
#include <mini/tor/hidden_service.h>
#include <mini/logger.h>
#include <mini/io/stream_reader.h>
#include <atomic>

class tor_client
{
  public:
    tor_client(
      void
      )
      : _consensus(mini::tor::consensus::option_flag::do_not_use_cache)
    {

    }

    ~tor_client(
      void
      )
    {
      delete _circuit;
    }

    void
    extend_to(
      const mini::string_ref onion_router_name
      )
    {
      mini::tor::onion_router* router = _consensus.get_onion_router_by_name(onion_router_name);

      if (_circuit == nullptr)
      {
        _socket.connect(router);
        _circuit = _socket.create_circuit();
      }
      else
      {
        _circuit->extend(router);
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

      mini_info("tor_client::http_get() fetching [%s]", url_string.get_buffer());
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

        stream = _circuit->create_onion_stream(onion, port);
      }
      else
      {
        extend_to("colocall321");

        stream = _circuit->create_stream(host, port);
      }

      mini::string req = "GET " + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";
      stream->write(req.get_buffer(), req.get_size());

      mini::io::stream_reader sr(*stream);
      result = sr.read_string_to_end();

      delete stream;

      return result;
    }

  private:
    mini::tor::consensus _consensus;
    mini::tor::tor_socket _socket;
    mini::tor::circuit* _circuit = nullptr;
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

  tor_client tor;
  tor.extend_to("tor26");

  mini::string content = tor.http_get(0 ? "http://duskgytldkxiuqc6.onion/fedpapers/federndx.htm" : argv[1]);
  mini::console::write("%s", content.get_buffer());

  mini::console::write("\n");
  mini::console::write("-----------------------------\n");
  mini::console::write("content size: %u bytes\n", content.get_size());
  mini::console::write("-----------------------------\n");

  return 0;
}