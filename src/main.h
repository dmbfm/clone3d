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
    int x_pos;
    int y_pos;
    int last_x_pos;
    int last_y_pos;
    int delta_x;
    int delta_y;
} MouseState;

typedef struct
{
    KeyState key_left;
    KeyState key_right;
    KeyState key_up;
    KeyState key_down;
    KeyState key_esc;

    MouseState mouse;
} InputState;

inline void key_state_set(KeyState *k, bool32 is_down, bool32 was_down)
{
    k->is_down = is_down;
    k->was_down = was_down;
}

typedef struct
{
    void *buffer;
    uint64 size;
} ReadFileResult;

typedef struct
{
    void (*print_string)(const char *str);
    bool32 (*read_entire_file)(const char *str, ReadFileResult *result);

} PlatformLayer;


typedef struct
{
    uint32 size_in_bytes;
    int32 width;
    int32 height;
    uint16 bits_per_pixel;
    int32 row_size;
    void *pixels;
} Bitmap;

typedef struct {
    int offsetX;
    int offsetY;
    
    real32 playerX;
    real32 playerY;
    // TODO: make this real32, or, make that we go from 0 to 36000 or something, instead
    // of 0, 360
    int16 player_view_angle;
} State;

void Update(Video *video, PlatformLayer *platform, Memory *memory, InputState *input);

#endif
