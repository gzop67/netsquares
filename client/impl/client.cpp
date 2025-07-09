#include "../../defines.h"
#include "../client.h"
#include "../../packets.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

internal SOCKET udp_socket;
internal struct addrinfo *server_addr_info;
internal bool8 has_connection;

internal void
bind_for_udp()
{
  struct addrinfo hints;
  struct addrinfo *address_info;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  if (getaddrinfo("127.0.0.1", GAME_PORT, &hints, &address_info) != 0)
  {
    assert(FALSE && "_getaddrinfo()_ fail\n");
  }

  udp_socket = socket(address_info->ai_family,
      address_info->ai_socktype, address_info->ai_protocol);
  if (udp_socket == INVALID_SOCKET)
  {
    assert(FALSE && "udp socket fail.");
  }

  /*
  if (bind(udp_socket, address_info->ai_addr, address_info->ai_addrlen)
      == SOCKET_ERROR)
  {
    wchar_t *s = NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&s, 0, NULL);
    fprintf(stdout, "%S\n", s);
    assert(FALSE && "udp bind failed.\n");
  }
  */
  freeaddrinfo(address_info);
}

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

  if (getaddrinfo("127.0.0.1", LOGIN_PORT, &hints, &server_addr_info) != 0)
  {
    assert(FALSE && "Fail");
  }

  SOCKET s = socket(server_addr_info->ai_family, server_addr_info->ai_socktype,
      server_addr_info->ai_protocol);
  if (s == INVALID_SOCKET)
  {
    assert(FALSE && "Socket failed\n");
  }

  for (;;)
  {
    if (connect(s, server_addr_info->ai_addr, (int)server_addr_info->ai_addrlen) ==
        SOCKET_ERROR)
    {
    }
    else
    {
      fprintf(stdout, "Server connection found.\n");
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
              bind_for_udp();
              char ping[5] = {'p', 'i', 'n', 'g', '\0'};

              struct sockaddr_in addr;
              addr.sin_family = AF_INET;
              addr.sin_port = htons(atoi(GAME_PORT));
              addr.sin_addr.s_addr = inet_addr("127.0.0.1");
              s32 sz = sizeof(sockaddr_in);
              s32 r = sendto(udp_socket, ping, 4, 0, (sockaddr*)&addr, sz);
              if (r == SOCKET_ERROR)
              {
                wchar_t *s = NULL;
                FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                    NULL, WSAGetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPWSTR)&s, 0, NULL);
                fprintf(stdout, "%S\n", s);
              }
              else if (r > 0)
              {
                fprintf(stdout, "Sent login ack to switch to udp.\n");
              }
              else
              {
                assert(FALSE && "Fail send...");
              }
              has_connection = TRUE;
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
poll_server(void *)
{
  for (;;)
  {
    char buf[256];
    struct sockaddr_storage their_addr;
    socklen_t sz = sizeof(sockaddr_storage);
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
}

