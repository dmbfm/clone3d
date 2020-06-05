#if !defined(MAIN_H)
#define MAIN_H

#include <stdint.h>


#define global static
#define local_persist static

#define false 0
#define true !(false)

#define TEXTURE_RGB 0x00
#define TEXTURE_RGBA 0x01

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
    uint32 width;
    uint32 height;
    uint32 *pixels;
    uint16 format;
} Texture;

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

typedef struct {
    int offsetX;
    int offsetY;
    
    real32 playerX;
    real32 playerY;
    // TODO: make this real32, or, make that we go from 0 to 36000 or something, instead
    // of 0, 360
    int16 player_view_angle;

    uint32 pixels[10000];
} State;

void Update(Texture *video, PlatformLayer *platform, Memory *memory, InputState *input);

#endif
