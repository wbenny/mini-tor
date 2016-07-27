#pragma once
#include "http.h"

#include <windows.h>
#include <wininet.h>

#pragma comment (lib, "wininet.lib")
#pragma comment (lib, "ws2_32.lib")

#define USER_AGENT "Mozilla/5.0 (Windows NT 6.1; rv:45.0) Gecko/20100101 Firefox/45.0"

namespace mini::net::http::client {

string
get(
  const string_ref host,
  uint16_t port,
  const string_ref path
  )
{
  HINTERNET hInternet = InternetOpen(
    TEXT(USER_AGENT),
    INTERNET_OPEN_TYPE_PRECONFIG,
    NULL,
    NULL,
    0);

  HINTERNET hConnection = InternetConnect(
    hInternet,
    host.get_buffer(),
    port,
    TEXT(""),
    TEXT(""),
    INTERNET_SERVICE_HTTP,
    0,
    0);

  HINTERNET hData = HttpOpenRequest(
    hConnection,
    TEXT("GET"),
    path.get_buffer(),
    HTTP_VERSION,
    NULL,
    NULL,
    INTERNET_FLAG_DONT_CACHE,
    0);

  char buffer[256 * 1024 + 1];
  HttpSendRequestA(hData, NULL, 0, NULL, 0);

  DWORD bytes_read = 0;
  DWORD total_bytes_read = 0;

  string result;

  while (InternetReadFile(hData, buffer, sizeof(buffer) - 1, &bytes_read) && bytes_read != 0)
  {
    buffer[bytes_read] = 0;

    result += buffer;

    total_bytes_read += bytes_read;
  }

  InternetCloseHandle(hData);
  InternetCloseHandle(hConnection);
  InternetCloseHandle(hInternet);

  return result;
}

}
