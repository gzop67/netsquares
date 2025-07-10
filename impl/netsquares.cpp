#include "../defines.h"
#include "../win32.h"
#include "../netsquares.h"
#include "../client/client.h"

#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <timeapi.h>

internal u32 food_count;
internal v2i food[FOOD_MAX];
internal bool8 food_alive[FOOD_MAX];
internal RECT temp[FOOD_MAX];

internal v2i player_pos;

internal void
update()
{
  client_state_packet pack;
  pack._header._type = CLIENT_STATE;
  pack._header._size = sizeof(pack);
  pack._player_pos = player_pos;

  send_to_server((char*)&pack, sizeof(client_state_packet));
}

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

internal LARGE_INTEGER timing_first, timing_last, timing_freq;
internal void
dt_start()
{
  QueryPerformanceCounter(&timing_first);
}
internal f32
dt_end()
{
  QueryPerformanceCounter(&timing_last);
  f32 dt = (((f64)timing_last.QuadPart -
        (f64)timing_first.QuadPart)) / (f64)timing_freq.QuadPart;
  return (dt);
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
  QueryPerformanceFrequency(&timing_freq);
  timeBeginPeriod(1);

  HANDLE poll_thread = (HANDLE)_beginthread(poll_server, 0, NULL);
  for (;;)
  {
    update();
    win32_update();

    dt_start();
    f32 dt = dt_end();
    f32 cap_delta = WORLD_UPDATE_FREQ / 1000.0f;
    if (dt < cap_delta)
    {
      Sleep((cap_delta-dt) * 1000);
    }
  }
  timeEndPeriod(1);
  return (0);
}
