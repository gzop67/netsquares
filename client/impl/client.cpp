#include "../../defines.h"
#include "../client.h"
#include "../../packets.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

internal SOCKET udp_socket;
internal struct addrinfo *server_addr_info;

bool8
client_connect()
{
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
  {
    assert(FALSE && "Fail startup.");
  }

  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo("127.0.0.1", PORT, &hints, &server_addr_info) != 0)
  {
    assert(FALSE && "Fail");
  }

  SOCKET s = socket(server_addr_info->ai_family, server_addr_info->ai_socktype,
      server_addr_info->ai_protocol);
  if (s == INVALID_SOCKET)
    assert(FALSE && "Socket failed\n");


  for (;;)
  {
    if (connect(s, server_addr_info->ai_addr, (int)server_addr_info->ai_addrlen) ==
        SOCKET_ERROR)
    {
    }
    else
    {
      fprintf(stdout, "found connection\n");
      for (;;)
      {
        login_packet p;
        p._header._type = LOGIN;
        sprintf(p._username, "user1");
        sprintf(p._password, "pass123");
        char buf[1024];
        u32 size = make_packet(LOGIN, (void*)&p, buf);
        send(s, buf, size, 0);
        memset(buf, 0, 1024);
        for (;;)
        {
          if (recv(s, buf, 1024, 0) > 0)
          {
            if (buf[0] == 9)
            {
              fprintf(stdout, "Login accepted.\n");
              if (closesocket(s) == SOCKET_ERROR)
              {
                assert(FALSE && "Failed to close socket on client connect.");
              }

              struct addrinfo hints;
              struct addrinfo *address_info;
              memset(&hints, 0, sizeof(struct addrinfo));
              hints.ai_family = AF_UNSPEC;
              hints.ai_socktype = SOCK_DGRAM;
              if (getaddrinfo("127.0.0.1", PORT, &hints, &address_info) != 0)
              {
                fprintf(stdout, "faili\n");
                assert(FALSE && "Addr fail\n");
              }
              udp_socket = socket(address_info->ai_family,
                  address_info->ai_socktype, address_info->ai_protocol);
              if (udp_socket == INVALID_SOCKET)
              {
                assert(FALSE && "UDP SOCK FAIL.");
              }

              if (bind(udp_socket, address_info->ai_addr, address_info->ai_addrlen) == SOCKET_ERROR)
              {
                fprintf(stdout, "fail bind\n");
                assert(FALSE && "Bind fail\n");
              }
              return (TRUE);
            }
          }
        }
      }
    }
  }
  return (FALSE);
}

void
poll_server()
{
  char buf[256];
  struct sockaddr_storage their_addr;
  socklen_t sz = sizeof(their_addr);
  fprintf(stdout, "recvn?\n");
  s32 r = recvfrom(udp_socket, buf, 256-1, 0, (sockaddr*)&their_addr,
      &sz);
  if (r > 0 && r != SOCKET_ERROR)
  {
    fprintf(stdout, ":: %s\n", buf);
  }
  else
  {
    wchar_t *s = NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&s, 0, NULL);
    fprintf(stdout, "%S\n", s);
    assert(FALSE && "Recvfrom fail.");
  }
}

