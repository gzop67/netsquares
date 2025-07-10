#ifndef SERVER_WORLD_H
#define SERVER_WORLD_H

#include "../defines.h"
bool8 spawn_food(v2i pos);
void world_update();

typedef struct player player;
struct player
{
  u32 _client_id;
  v2i _pos;
};

void player_update(player p);
void player_join(player p);

#endif// SERVER_WORLD_H
