#if !defined(MAIN_H)

#include <stdint.h>

#define MAIN_H

#define global static
#define local_persist static

#define false 0
#define true !(false)

#define KB(x) (1024*x)
#define MB(x) (1024*KB(x))
#define GB(x) (1024*MB(x))
#define TB(x) (1024*TB(x))


typedef float real32;
typedef double real64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int32 bool32;

typedef struct
{
    uint32 *framebuffer;
    uint32 width;
    uint32 height;
    uint32 bytes_per_pixel;

    // NOTE(daniel): stride is the number of bytes in a pixel row, which may include a
    // padding.
    uint32 stride;
    uint32 total_size_in_bytes;
} Video;

typedef struct
{
    void *storage;
    uint64 size;
    bool32 is_initialized;
} Memory;

typedef struct
{
    bool32 is_down;
    bool32 was_down;
} KeyState;

typedef struct
{
    KeyState key_left;
    KeyState key_right;
    KeyState key_up;
    KeyState key_down;
} InputState;

inline void key_state_set(KeyState *k, bool32 is_down, bool32 was_down)
{
    k->is_down = is_down;
    k->was_down = was_down;
}

typedef struct
{
    void (*print_string)(const char *str);
} PlatformLayer;

void Update(Video *video, PlatformLayer *platform, Memory *memory, InputState *input);

#endif
