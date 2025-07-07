#ifndef PACKETS_H
#define PACKETS_H

#include "defines.h"

typedef enum PACKET_TYPE : u8
{
  ACK,
  LOGIN,
  HEARTBEAT
} PACKET_TYPE;

typedef struct net_header net_header;
struct net_header
{
  u8 _type;
  u8 _padd[3];
};

typedef struct login_packet login_packet;
struct login_packet
{
  net_header _header;
  char _username[32];
  char _password[32];
};

u32 make_packet(enum PACKET_TYPE type, void *packet, char *dst);

#endif// PACKETS_H
