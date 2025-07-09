#include "../world.h"
#include "../server.h"
#include "../../packets.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

internal u32 food_count;
internal v2i food_pos[FOOD_MAX];
internal bool8 food_alive[FOOD_MAX];

bool8
spawn_food(v2i pos)
{
  for (u32 i = 0; i < FOOD_MAX; i++)
  {
    if (!food_alive[i])
    {
      food_alive[i] = TRUE;
      food_pos[i] = pos;
      food_count++;
      return (TRUE);
    }
  }
  return (FALSE);
}

void
world_update()
{
  world_state_packet world_state;
  world_state._header._type = WORLD_STATE;
  world_state._header._size = sizeof(world_state_packet);
  world_state._food_count = food_count;
  memcpy(world_state._food_pos, food_pos, sizeof(v2i) * FOOD_MAX);
  send_packet_to_all_clients(&world_state, sizeof(world_state));

  Sleep(5000);
  fprintf(stdout, "%d\n", rand());
  if (rand() % 100 < 10)
  {
    spawn_food({(u32)rand() % WINDOW_WIDTH, (u32)rand() % WINDOW_HEIGHT});
  }
}
