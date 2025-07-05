#include "../defines.h"
#include "../win32.h"
#include "../netsquares.h"

#include <stdio.h>

int main(int argc, char **argv)
{
  win32_initialise();
  for (;;)
  {
    if (client_connect())
    {
      fprintf(stdout, "Client connected, exiting.");
      return (0);
    }
  }
  return (0);
}
