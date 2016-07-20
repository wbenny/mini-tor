#pragma once
#include "http.h"

#include <windows.h>
#include <wininet.h>

#pragma comment (lib, "wininet.lib")
#pragma comment (lib, "ws2_32.lib")

#define USER_AGENT "Mozilla/5.0 (Windows NT 6.1; rv:45.0) Gecko/20100101 Firefox/45.0"

/*
int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup failed.\n";
		system("pause");
		return 1;
	}
	SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct hostent *host;
	host = gethostbyname("194.109.206.212");
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
	cout << "Connecting...\n";
	if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) {
		cout << "Could not connect";
		system("pause");
		return 1;
	}
	cout << "Connected.\n";
	char bts[] = "GET /tor/status-vote/current/consensus HTTP/1.0\r\n\r\n";
	send(Socket, bts, strlen(bts), 0);
	char buffer[10000];
	int nDataLength;
	while ((nDataLength = recv(Socket, buffer, 10000, 0)) > 0) {
		int i = 0;
		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
			cout << buffer[i];
			i += 1;
		}
	}
	closesocket(Socket);
	WSACleanup();
	system("pause");
	return 0;
}
*/

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

  char buf[128*1024 + 1];
  HttpSendRequestA(hData, NULL, 0, NULL, 0);

  DWORD bytesRead = 0;
  DWORD totalBytesRead = 0;

  string result;

  while (InternetReadFile(hData, buf, sizeof(buf) - 1, &bytesRead) && bytesRead != 0)
  {
    buf[bytesRead] = 0;

    result += buf;

    totalBytesRead += bytesRead;
  }

  InternetCloseHandle(hData);
  InternetCloseHandle(hConnection);
  InternetCloseHandle(hInternet);

  return result;
}

}
