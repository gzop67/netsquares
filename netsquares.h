#ifndef NETSQUARES_H
#define NETSQUARES_H
#include <windows.h>

void get_player_rect(RECT *rect);
u32 get_world_rects(RECT *world_rects);
void move_player(v2f by);

#endif// NETSQUARES_H
