#ifndef PACKETS_H
#define PACKETS_H


#include "defines.h"

typedef enum PACKET_TYPE : u8
{
  ACK,
  LOGIN,
  HEARTBEAT,
};

typedef struct net_header net_header;
struct net_header
{
};

typedef struct login_packet login_packet;
struct login_packet
{
};

bool8 make_packet(enum PACKET_TYPE type, void *packet);

#endif// PACKETS_H
