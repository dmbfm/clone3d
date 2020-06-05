#if !defined(GRAPHICS_H)
#define GRAPHICS_H

#include "main.h"

#define M_PI 3.141592653589f


#define PACK_RGBA(r, g, b, a) ((uint32) ((((uint8)a) << 24) | (((uint8)b) << 16) | (((uint8)g) << 8) | ((uint8)r)))
#define PACK_RGB(r, g, b) ((uint32) ((255 << 24) | (((uint8)b) << 16) | (((uint8)g) << 8) | ((uint8)r)))

#define COLOR_BLUE PACK_RGB(0, 0, 255)
#define COLOR_GREEN PACK_RGB(0, 255, 0)
#define COLOR_RED PACK_RGB(255, 0, 0)
#define COLOR_BLACK PACK_RGB(0, 0, 0)
#define COLOR_WHITE PACK_RGB(255, 255, 255)

typedef struct {
    uint8 r;
    uint8 g;
    uint8 b;
    uint8 a;
} Color;

typedef struct
{
    uint32 size_in_bytes;
    int32 width;
    int32 height;
    uint16 bits_per_pixel;
    int32 row_size;
    void *pixels;
} Bitmap;

inline bool32 check_pixel_coords(Texture *tex, int x, int y);
void put_pixel(Texture *tex, int x, int y, uint32 color);
void draw_texture(Texture *src, Texture *dst, int x0, int y0);
void draw_rectangle(Texture *tex, int x, int y, int width, int height, uint32 color);
void draw_line(Texture *tex, int x0, int y0, int x, int y, uint32 color);
void texture_get_rgba(Texture *tex, int x, int y, uint8 *r, uint8 *g, uint8 *b, uint8 *a);
void texture_get_rgba(Texture *tex, int x, int y, uint8 *r, uint8 *g, uint8 *b, uint8 *a);
#endif
