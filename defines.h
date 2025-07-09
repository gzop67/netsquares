#ifndef NETSQUARES_DEFINES_H
#define NETSQUARES_DEFINES_H

#define WIN32_LEAN_AND_MEAN //to stop using old version of berkeley sockets.
#include <stdint.h>
#include <assert.h>

#define LOGIN_PORT "6665"
#define GAME_PORT "6666"

#define TRUE 1
#define FALSE 0

#define WINDOW_WIDTH 256
#define WINDOW_HEIGHT 256

#define SQUARE_SIZE 32

#define internal static
#define global static
#define local_persist static

typedef uint32_t bool32;
typedef uint8_t bool8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef float f32;
typedef double f64;

#endif// NETSQUARES_DEFINES_H
