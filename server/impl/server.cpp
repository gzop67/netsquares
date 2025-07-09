#include "../server.h"
#include "../../packets.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <stdio.h>
#include <cstdlib>

#define MAX_LOGIN_CONNECTIONS 16
#define MAX_CLIENTS 32

typedef struct login_connection login_connection;
struct login_connection
{
  SOCKET _socket;
  sockaddr_storage _sockaddr_storage;
};

internal login_connection login_connections[MAX_LOGIN_CONNECTIONS];
internal SOCKET udp_socket;

typedef struct client_connection client_connection;
struct client_connection
{
  u32 _session_id;
  u32 _client_id;
  sockaddr_storage _address_info;
};

typedef struct client_connection_tracker client_connection_tracker;
struct client_connection_tracker
{
  client_connection _client_connection;
  u32 _ms_since_last_heartbeat;
  bool8 _alive;
};

internal client_connection_tracker connected_clients[MAX_CLIENTS];

internal void
winsock_err(const char *msg)
{
  wchar_t *s = NULL;
  FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&s, 0, NULL);
  fprintf(stdout, "From %s, failed with : %S\n", msg, s);
  assert(FALSE && "");
}

internal u32
dumb_hash(const char *c)
{
  u32 hash = 0;
  char *t = &((char*)c)[0];
  while (*t != '\0')
  {
    hash += *t;
    t++;
  }
  return (hash);
}
internal u32
random_u32()
{
  /*
   * FIX (12:27PM 250707):
   *   Dumb temp code.
   * */
  local_persist u32 counter;
  counter += 17;
  return (counter);
}

internal
s32 get_empty_login_socket_index()
{
  for (u32 i = 0; i < MAX_LOGIN_CONNECTIONS; i++)
  {
    if (login_connections[i]._socket == INVALID_SOCKET)
    {
      return (i);
    }
  }
  return (-1);
}

//TEMP passing username here.
internal bool8
connect_client(u32 login_connections_index, const char *username)
{
  login_connection *lc = &login_connections[login_connections_index];
  client_connection cc;
  cc._client_id = dumb_hash(username);
  cc._session_id = random_u32();

  for (u32 i = 0; i < MAX_CLIENTS; i++)
  {
    if (!connected_clients[i]._alive)
    {
      connected_clients[i]._alive = TRUE;
      connected_clients[i]._ms_since_last_heartbeat = 0;

      char buf[16];
      buf[0] = 9;
      send(lc->_socket, buf, 16, 0);

      struct sockaddr_storage their_addr;
      socklen_t sz = sizeof(their_addr);

      s32 r = recvfrom(udp_socket, buf, 16-1, 0, (sockaddr*)&their_addr,
          &sz);
      if (r > 0 && r != SOCKET_ERROR)
      {
        fprintf(stdout, "ACK:: %s\n", buf);
        cc._address_info = *(sockaddr_storage*)&their_addr;
        connected_clients[i]._client_connection = cc;
      }
      else
      {
        fprintf(stdout, "prob timedout.\n");
        winsock_err("ping recvfrom()");
      }
      if (closesocket(lc->_socket) == SOCKET_ERROR)
      {
        assert(FALSE && "Failed to close socket on client connect.");
      }
      else
      {
        fprintf(stdout, "Client logged in.");
        return (TRUE);
      }
    }
  }
  assert(FALSE && "Ran out of connection bandwidth.");
  return (FALSE);
}

void
server_maintain_connections(void*)
{
  char ping[5] = {'p', 'i', 'n', 'g', '\0'};

  for (;;)
  {
    Sleep(1000);
    for (u32 i = 0; i < MAX_CLIENTS; i++)
    {
      if (connected_clients[i]._alive)
      {
        client_connection_tracker *c = &connected_clients[i];

        s32 sz = sizeof(sockaddr_storage);
        s32 r = sendto(udp_socket, ping, 4, 0,
              (sockaddr*)&c->_client_connection._address_info,
              sz);
        if (r == SOCKET_ERROR)
        {
          winsock_err("udp sendto()");
        }
        else
        {
        }

        sockaddr_in *sin = (sockaddr_in*)&c->_client_connection._address_info;
        char hname[128];
        getnameinfo((sockaddr*)&c->_client_connection._address_info,
            sizeof(c->_client_connection._address_info), hname, 128, NULL, 0,
            0);
        fprintf(stdout, "packet sent to: %s on %s:%d\n", 
            hname, inet_ntoa(sin->sin_addr), sin->sin_port);

      }
    }
  }
}

void
server_handle_logins(void *)
{
  for (;;)
  {
    for (u32 i = 0; i < MAX_LOGIN_CONNECTIONS; i++)
    {
      if (login_connections[i]._socket != INVALID_SOCKET)
      {
        char buf[1024];
        u32 c = recv(login_connections[i]._socket, buf, 1024, 0);
        if (c > 0)
        {
          login_packet p = *(login_packet*)buf;
          if (db_lookup(p._username, p._password))
          {
            if (connect_client(i, p._username))
            {
              login_connections[i]._socket = INVALID_SOCKET;
            }
            else
            {
              assert(FALSE && "Failed client connection.");
            }
          }
        }
      }
    }
  }
}

void
server_listen_for_new_connections(void*)
{
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
  {
    assert(FALSE && "_WSAStartup()_ failed.");
  }
  struct addrinfo hints;
  struct addrinfo *address_info;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo("127.0.0.1", LOGIN_PORT, &hints, &address_info) != 0)
  {
    winsock_err("getaddrinfo()");
  }

  SOCKET tcp_socket = socket(address_info->ai_family, address_info->ai_socktype,
      address_info->ai_protocol);
  if (tcp_socket == INVALID_SOCKET)
  {
    winsock_err("socket()");
  }

  if (bind(tcp_socket, address_info->ai_addr, address_info->ai_addrlen) == SOCKET_ERROR)
  {
    winsock_err("bind()");
  }

  if (listen(tcp_socket, 5) == SOCKET_ERROR)
  {
    winsock_err("listen()");
  }

  freeaddrinfo(address_info);
  fprintf(stdout, "Listening for client connections.\n");
  for (;;)
  {
    struct sockaddr_storage their_addr;
    socklen_t sz = sizeof(sockaddr_storage);
    SOCKET connection_socket;

    if ((connection_socket = accept(tcp_socket, (sockaddr*)&their_addr, &sz))
        != INVALID_SOCKET)
    {
      s32 sid = get_empty_login_socket_index();
      if (sid != -1)
      {
        char hname[128];
        sockaddr_in *sin = (sockaddr_in*)&their_addr;
        getnameinfo((sockaddr*)&their_addr, sizeof(their_addr), hname, 128,
            NULL, 0, 0);
        fprintf(stdout, "New connection: %s on %s:%d\n", 
            hname, inet_ntoa(sin->sin_addr), sin->sin_port);
        login_connections[sid]._socket = connection_socket;
        login_connections[sid]._sockaddr_storage = their_addr;
      }
      else
      {
        winsock_err("accept()");
      }
    }
    else
    {
    }
  }
}

internal HANDLE server_function_threads[3];

int
main (int argc, char **argv)
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
  hints.ai_socktype = SOCK_DGRAM;
  if (getaddrinfo("127.0.0.1", GAME_PORT, &hints, &address_info) != 0)
  {
    winsock_err("getaddrinfo()");
  }
  udp_socket = socket(address_info->ai_family,
      address_info->ai_socktype, address_info->ai_protocol);
  if (udp_socket == INVALID_SOCKET)
  {
    winsock_err("udp socket()");
  }
  if (bind(udp_socket, address_info->ai_addr, address_info->ai_addrlen)
      == SOCKET_ERROR)
  {
    winsock_err("udp bind()");
  }

  freeaddrinfo(address_info);
  for (u32 i = 0; i < MAX_LOGIN_CONNECTIONS; i++)
  {
    login_connections[i]._socket = INVALID_SOCKET;
  }
  server_function_threads[0] = (HANDLE)_beginthread(
     server_listen_for_new_connections, 0, NULL);
  if (!server_function_threads[0])
  {
    assert(FALSE && "Failed to create listen_for_new_connections thread.");
  }
  server_function_threads[1] = (HANDLE)_beginthread(
     server_handle_logins, 0, NULL);
  if (!server_function_threads[1])
  {
    assert(FALSE && "Failed to create server_handle_logins thread.");
  }

  server_function_threads[2] = (HANDLE)_beginthread(
     server_maintain_connections, 0, NULL);
  if (!server_function_threads[2])
  {
    assert(FALSE && "Failed to create server_maintain thread.");
  }
  WaitForMultipleObjects(3, server_function_threads, TRUE, INFINITE);

  return (0);
}
