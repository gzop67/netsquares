#ifndef NETSQUARES_H
#define NETSQUARES_H
#include <windows.h>
#include "packets.h"

void get_player_rect(RECT *rect);
u32 get_world_rects(RECT *world_rects);
void move_player(v2f by);
void set_world_state(world_state_packet *packet);

#endif// NETSQUARES_H
