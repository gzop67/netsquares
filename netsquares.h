#ifndef NETSQUARES_H
#define NETSQUARES_H
#include <windows.h>
#include "packets.h"

u32 get_players(RECT *rects);
u32 get_world_rects(RECT *world_rects);
void move_player(v2f by);
void set_world_state(world_state_packet *packet);

#endif// NETSQUARES_H
