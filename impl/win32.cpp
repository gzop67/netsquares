#include "../defines.h"
#include "../win32.h"
#include "../netsquares.h"
#include <windows.h>

internal bool8 keys[4]; //up, down, left, right
internal HWND hwnd;
internal HBRUSH fill, world_col, player_col;

LRESULT CALLBACK
window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_ERASEBKGND:
      {
      } break;
    case WM_PAINT:
      {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        FillRect(hdc, &ps.rcPaint, fill);

        RECT world_rects[64];
        RECT players[MAX_CLIENTS];
        u32 count = get_world_rects(world_rects);
        u32 player_count = get_players(players);
        for (u32 i = 0; i < count; i++)
        {
          FillRect(hdc, &world_rects[i], world_col);
        }
        for (u32 i = 0; i < player_count; i++)
        {
          FillRect(hdc, &players[i], player_col);
        }


        EndPaint(hwnd, &ps);
      } break;
    case WM_KEYDOWN: 
      {
        if (wParam == VK_UP)
        {
          keys[0] = TRUE;
        }
        if (wParam == VK_DOWN)
        {
          keys[1] = TRUE;
        }
        if (wParam == VK_LEFT)
        {
          keys[2] = TRUE;
        }
        if (wParam == VK_RIGHT)
        {
          keys[3] = TRUE;
        }
      } break;
    case WM_KEYUP:
    {
        if (wParam == VK_UP)
        {
          keys[0] = FALSE;
        }
        if (wParam == VK_DOWN)
        {
          keys[1] = FALSE;
        }
        if (wParam == VK_LEFT)
        {
          keys[2] = FALSE;
        }
        if (wParam == VK_RIGHT)
        {
          keys[3] = FALSE;
        }
      } break;
    case WM_SETCURSOR: 
      {
      } break;
    case WM_CREATE:
                         {
                         } break;
    case WM_CLOSE:
                     {
                       DestroyWindow(hwnd);
                     } break;
    case WM_DESTROY:
                     {
                       UnregisterClass("Netsquares", GetModuleHandle(NULL));
                     } break;
    default:
                     {
                       return DefWindowProc(hwnd, msg, wParam, lParam);
                     } break;
  }
  return 0;
}

void
win32_initialise()
{
  HINSTANCE instance = GetModuleHandle(NULL);

  WNDCLASS wnd = {0};
  wnd.hInstance = instance;
  wnd.hbrBackground = NULL;//(HBRUSH)(COLOR_BACKGROUND);
  wnd.lpfnWndProc = window_proc;
  wnd.lpszClassName = "Netsquares";
  wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
  wnd.style = CS_VREDRAW | CS_HREDRAW;

  RegisterClass(&wnd);

  RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = WINDOW_WIDTH;
  rect.bottom = WINDOW_HEIGHT;
  /*
   * DETAIL:<(jack): WS_OVERLAPPED is not valid for _AdjustWindowRect_
   * as per docs.
   */
  DWORD win_style = WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | 
                     WS_MINIMIZEBOX | WS_SIZEBOX;

  AdjustWindowRect(&rect, win_style, FALSE);

  hwnd = CreateWindowEx(
      0,//0x00200000L,
      wnd.lpszClassName,
      "netsquares", 
      win_style, 
      CW_USEDEFAULT, CW_USEDEFAULT, 
      rect.right - rect.left,
      rect.bottom - rect.top,
      NULL, NULL, instance, NULL);

  if (!hwnd)
    assert("Win32 window creation failed.");

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  world_col = CreateSolidBrush(RGB(96,0,64));
  player_col = CreateSolidBrush(RGB(255,0,0));
  fill = CreateSolidBrush(RGB(0,0,0));
}

bool8
win32_update()
{
  MSG msg;
  while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  RECT rect;
  GetClientRect(hwnd, &rect);
  InvalidateRect(hwnd, &rect, TRUE);

  if (keys[0])
  {
    move_player({0, -MOVE_SPEED});
  }
  if (keys[1])
  {
    move_player({0, MOVE_SPEED});
  }
  if (keys[2])
  {
    move_player({-MOVE_SPEED, 0});
  }
  if (keys[3])
  {
    move_player({MOVE_SPEED, 0});
  }


  return (TRUE);
}

