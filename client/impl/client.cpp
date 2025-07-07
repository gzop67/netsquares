#include "../../defines.h"
#include "../client.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

bool8
client_connect()
{
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
  {
    assert(FALSE && "Fail startup.");
  }

  struct addrinfo hints;
  struct addrinfo *address_info;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo("127.0.0.1", PORT, &hints, &address_info) != 0)
  {
    assert(FALSE && "Fail");
  }

  SOCKET s = socket(address_info->ai_family, address_info->ai_socktype,
      address_info->ai_protocol);
  if (s == INVALID_SOCKET)
    assert(FALSE && "Socket failed\n");


  for (;;)
  {
    if (connect(s, address_info->ai_addr, (int)address_info->ai_addrlen) ==
        SOCKET_ERROR)
    {
    }
    else
    {
      fprintf(stdout, "found connection\n");
      for (;;)
      {
        char msg[] = "hello";
        send(s, msg, sizeof(msg), 0);
      }
    }
  }

  freeaddrinfo(address_info);

  return (TRUE);
}
