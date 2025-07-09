#include "../defines.h"
#include "../win32.h"
#include "../netsquares.h"
#include "../client/client.h"

#include <stdio.h>
#include <windows.h>
#include <process.h>

int main(int argc, char **argv)
{
  win32_initialise();
  for (;;)
  {
    if (client_connect())
    {
      fprintf(stdout, "Client connected.\n");
      break;
    }
  }
  HANDLE poll_thread = (HANDLE)_beginthread(poll_server, 0, NULL);
  for (;;)
  {
    win32_update();
  }
  return (0);
}
