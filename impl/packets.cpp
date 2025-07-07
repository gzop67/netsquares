#include "../packets.h"
#include <cstring>

u32
make_packet(enum PACKET_TYPE type, void *packet, char *dst)
{
  switch (type)
  {
    case ACK:
      {

      } break;
    case LOGIN:
      {
        memcpy(dst, (char*)packet, sizeof(login_packet));
        return (sizeof(login_packet));
      } break;

    case HEARTBEAT:
      {

      } break;
  }
  return (FALSE);
}
