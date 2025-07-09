#include "../defines.h"
#include "../win32.h"
#include "../netsquares.h"
#include "../client/client.h"

#include <stdio.h>
#include <windows.h>
#include <process.h>

internal u32 food_count;
internal v2i food[FOOD_MAX];
internal bool8 food_alive[FOOD_MAX];
internal RECT temp[FOOD_MAX];

internal v2f player_pos;

void
set_world_state(world_state_packet *packet)
{
  food_count = packet->_food_count;
  memcpy(food, packet->_food_pos, sizeof(v2i) * FOOD_MAX);
  memset(food_alive, 0, sizeof(bool8) * FOOD_MAX);
  for (u32 i = 0; i < food_count; i++)
  {
    food_alive[i] = TRUE;
    v2i p = food[i];
    temp[i] = {(s64)(p._x - FOOD_SIZE*0.5f), 
      (s64)(p._y + FOOD_SIZE*0.5f),
      (s64)(p._x + FOOD_SIZE*0.5f),
      (s64)(p._y - FOOD_SIZE*0.5f)};
  }
}

void
get_player_rect(RECT *rect)
{
  *rect = {
    (s64)(player_pos._x - PLAYER_SIZE*0.5f), 
    (s64)(player_pos._y + PLAYER_SIZE*0.5f),
    (s64)(player_pos._x + PLAYER_SIZE*0.5f),
    (s64)(player_pos._y - PLAYER_SIZE*0.5f)};
}
void
move_player(v2f by)
{
  player_pos._x += by._x;
  player_pos._y += by._y;
}
u32
get_world_rects(RECT *world_rects)
{
  u32 start = 0;
  for (u32 i = 0; i < food_count; i++)
  {
    for (u32 j = start; j < FOOD_MAX; j++)
    {
      if (food_alive[j])
      {
        world_rects[i] = temp[j];
        start++;
        break;
      }
    }
  }
  return (food_count);
}

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
