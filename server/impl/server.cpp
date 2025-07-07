#include "../server.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <processthreadsapi.h>
#include <stdio.h>

#define MAX_LOGIN_SOCKETS 16

internal SOCKET login_sockets[MAX_LOGIN_SOCKETS];

internal
s32 get_empty_login_socket_index()
{
  for (u32 i = 0; i < MAX_LOGIN_SOCKETS; i++)
  {
    if (login_sockets[i] != INVALID_SOCKET)
    {
      return (i);
    }
  }
  return (-1);
}

DWORD WINAPI
server_handle_logins(LPVOID lpParam)
{
  for (;;)
  {
    for (u32 i = 0; i < MAX_LOGIN_SOCKETS; i++)
    {
      if (login_sockets[i] != INVALID_SOCKET)
      {
        char buf[1024];
        recv(login_sockets[i], buf, 1024, 0);
      }
    }
  }
  return (0);
}

DWORD WINAPI
server_listen_for_new_connections(LPVOID lpParam)
{
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
  {
    fprintf(stdout, "WTF\n");
    assert(FALSE && "Fail startup.");
  }
  struct addrinfo hints;
  struct addrinfo *address_info;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo("127.0.0.1", PORT, &hints, &address_info) != 0)
  {
    fprintf(stdout, "faili\n");
    assert(FALSE && "Addr fail\n");
  }

  SOCKET s = socket(address_info->ai_family, address_info->ai_socktype,
      address_info->ai_protocol);
  if (s == INVALID_SOCKET)
    assert(false && "Socket failed\n");


  if (bind(s, address_info->ai_addr, address_info->ai_addrlen) == SOCKET_ERROR)
  {
    fprintf(stdout, "fail bind\n");
    assert(FALSE && "Bind fail\n");
  }

  if (listen(s, 5) == SOCKET_ERROR)
  {
    fprintf(stdout, "Fail\n");
    assert(FALSE && "Listen fail\n");
  }

  freeaddrinfo(address_info);
  fprintf(stdout, "waiting to accept\n");
  for (;;)
  {
    struct sockaddr_storage their_addr;
    socklen_t sz = sizeof(sockaddr_storage);
    SOCKET connection_socket;

    if ((connection_socket = accept(s, (sockaddr*)&their_addr, &sz)) != INVALID_SOCKET)
    {
      s32 sid = get_empty_login_socket_index();
      if (sid != -1)
      {
        fprintf(stdout, "New connection found.\n");
        login_sockets[sid] = connection_socket;
      }
      else
      {
        assert(FALSE && "Err");
      }
    }
    else
    {
      //new connection accepted.
    }
  }
  return (0);
}

internal HANDLE server_function_threads[2];

int
main (int argc, char **argv)
{
  for (u32 i = 0; i < MAX_LOGIN_SOCKETS; i++)
  {
    login_sockets[i] = INVALID_SOCKET;
  }
  server_function_threads[0] = CreateThread(
     NULL, 0, server_listen_for_new_connections, NULL, 0, NULL);
  if (!server_function_threads[0])
  {
    assert(FALSE && "Failed to create listen_for_new_connections thread.");
  }
  server_function_threads[1] = CreateThread(
     NULL, 0, server_handle_logins, NULL, 0, NULL);
  if (!server_function_threads[0])
  {
    assert(FALSE && "Failed to create server_handle_logins thread.");
  }
  WaitForMultipleObjects(2, server_function_threads, TRUE, INFINITE);

  return (0);
}
