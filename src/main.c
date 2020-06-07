#include <stdio.h>
#include <math.h>
#include "main.h"
#include "math.h"
#include "graphics.h"


// TODO:
// - All bitmap code assumes bbp = 24 (3 bytes), make this more flexible?
// - Textured walls
// - Textured floors
// - Think about units
// - Make map size more flexible (or maybe just larger)


#define DebugLog(P, STR, ...)				\
{ char buf[256]; snprintf(buf, 256, STR, __VA_ARGS__); P->print_string(buf); }


global State *state;
global Bitmap test_bitmap;
global Texture test_bitmap_texture;

void test_tex_sample(Bitmap *bitmap, real32 u, real32 v, Color *c)
{
    int x = round32(u * (bitmap->width-1));
    int y = round32(v * (bitmap->height-1));

    uint8 *pixel = &((uint8 *)bitmap->pixels)[(y * bitmap->row_size) + (3 * x)];
    uint8 b = pixel[0];
    uint8 g = pixel[1];
    uint8 r = pixel[2];

    c->r = r;
    c->b = b;
    c->g = g;
    c->a = 255;
}

void draw_column(Texture *video, uint32 x, uint32 height, real32 r, real32 g, real32 b, real32 u, Texture *texture)
{
    if (x >= video->width || height == 0)
    {
        return;
    }
    
    uint32 hh = (video->height / 2);
    
    if (height > hh)
    {
        height = hh;
    }
    
    uint32 y0 = hh - height;
    uint32 ymax = hh + height;

    real32 v = 0;
    real32 v_step = 0.5f / height;
    
    for (uint32 j = y0; j < ymax; ++j)
    {
        uint32 *pixel = &(video->pixels[j * video->width + x]);

	int tx = round32(u * (texture->width-1));
	int ty = round32(v * (texture->height-1));
	
	//uint8 *tpixel = &((uint8 *)texture->pixels)[(ty * texture->row_size) + (3 * tx)];
	uint8 *texture_pixel = (uint8 *) &texture->pixels[ty * texture->width + tx];

	uint8 _r = *texture_pixel++;
	uint8 _g = *texture_pixel++;
	uint8 _b = *texture_pixel++;
        uint8 a = 255;

	    
        *pixel = (a << 24) | (_b << 16) | (_g << 8) | _r;

	v += v_step;
    }
}

void draw_map(Texture *video, const int *map, State *state, int size)
{
    Color c = {0};
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            int value = map[10 * (9 - j) + i];
            
            c.r = 255;
            c.g = value ? 0 : 255;
            c.b = value ? 0 : 255;
	    c.a = 255;
            
            draw_rectangle(video, size * i + 1, size * j+ 1, size, size, PACK_RGB(c.r, c.g, c.b));
        }
    }
}

void test_draw_bitmap(Bitmap *bitmap, Texture *video, PlatformLayer *platform)
{
    uint8 *pixels = (uint8 *) bitmap->pixels;

    Color c = {0, 0, 0, 255};

    for(int y = 0; y < bitmap->height; ++y)
    {
    	uint8 *row = (uint8 *) &pixels[y * bitmap->row_size];
	
    	for (int x = 0; x < bitmap->width; ++x)
    	{
	    uint32 base = 3 * x;
    	    c.b = row[base++];
    	    c.g = row[base++];
    	    c.r = row[base++];
    	    //c.a = row[base++];

	    put_pixel(video, x, y, PACK_RGB(c.r, c.g, c.b));
	    
    	}
    }
}

bool32 load_bitmap_from_file(PlatformLayer *p, const char *filename, Bitmap *bitmap)
{
    ReadFileResult result;
    if (!p->read_entire_file("Blue5.bmp", &result))
    {
	return false;
    }

    char *bytes = (char *) result.buffer;
    uint32 size = *((uint32 *)(bytes + 2));
    uint32 offset = *((uint32 *)(bytes+10));
    //uint32 comp = *((uint32 *)(bytes+30));
    int32 width = *((int32 *)(bytes+18));
    int32 height = *((int32 *)(bytes+22));
    int32 bpp = *((int32 *)(bytes+28));
    int32 row_size = ceil32((bpp * width) / 32.0f) * 4;
    
    void *pixels = (void *) (bytes + offset);

    bitmap->size_in_bytes = size;
    bitmap->width = width;
    bitmap->height = height;
    bitmap->bits_per_pixel = (uint16) bpp;
    bitmap->row_size = row_size;
    bitmap->pixels = pixels;

    return true;
}

bool32 load_bitmap_from_file_into_texture(PlatformLayer *p, const char *filename, Texture *texture)
{
    Bitmap bitmap = {0};

    if (!load_bitmap_from_file(p, filename, &bitmap))
    {
	return false;
    }

    texture->width = bitmap.width;
    texture->height = bitmap.height;
    texture->format = TEXTURE_RGB;

    // TODO: Memory arena 
    //texture->pixels = (uint32 *) malloc(texture->width * texture->height * sizeof(uint32));

    if (!texture->pixels)
    {
	return false;
    }

    uint8 *bitmap_pixels = (uint8 *) bitmap.pixels;
    for (int y = 0; y < bitmap.height; ++y)
    {
	uint8 *bitmap_row = &bitmap_pixels[bitmap.row_size * y];
	for (int x = 0; x < bitmap.width; ++x)
	{
	    uint8 *pixel = &bitmap_row[3 * x];

	    uint8 b = pixel[0];
	    uint8 g = pixel[1];
	    uint8 r = pixel[2];

	    texture->pixels[texture->width * y + x] = PACK_RGB(r, g, b);
	}
    }

    return true;
}   

void Update(Texture *video, PlatformLayer *platform, Memory *memory, InputState *input)
{
    
    // Origin: bottom left, y goes up
    local_persist const int map[10][10] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 1, 1, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
        {0, 1, 1, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    };
    
    
    if (!memory->is_initialized)
    {
        state = (State *) memory->storage;
        memory->is_initialized = true;
        
        state->offsetX = 200;
        state->offsetY = 200;
        
        state->playerX = 3.0f;
        state->playerY = 3.0f;

	// TODO: Memory arena
	test_bitmap_texture.pixels = (uint32 *) ((char *)memory->storage + sizeof(State));
	load_bitmap_from_file_into_texture(platform, "Blue5.bmp", &test_bitmap_texture);
    }
    
    if (input->key_left.is_down)
    {
        --state->offsetX;
        
        ++state->player_view_angle;
        if (state->player_view_angle >= 360)
        {
            state->player_view_angle = 0;
        }
    }
    
    if (input->key_right.is_down)
    {
        ++state->offsetX;
        
        --state->player_view_angle;
        if (state->player_view_angle < 0)
        {
            state->player_view_angle = 359;
        }
    }

    state->player_view_angle -= (int16) input->mouse.delta_x / 4;

    if (input->key_up.is_down)
    {
        ++state->offsetY;
        
        real32 fx = cosf(deg_to_rad(state->player_view_angle));
        real32 fy = sinf(deg_to_rad(state->player_view_angle));
        
        state->playerX += 0.025f * fx;
        state->playerY += 0.025f * fy;
        
        
        if (state->playerX > 10.0f)
        {
            state->playerX = 10.0f;
        }
        
        if (state->playerX < 0)
        {
            state->playerX = 0.0f;
        }
        
        if (state->playerY > 10.0f)
        {
            state->playerY = 10.0f;
        }
        
        if (state->playerY < 0)
        {
            state->playerY = 0.0f;
        }
    }
    
    if (input->key_down.is_down)
    {
        --state->offsetY;
    }
    
    const int tile_size = 8;

    draw_rectangle(video, 0, 0, video->width, video->height, PACK_RGB(0, 0, 0));
    draw_rectangle(video, 0, 0, video->width, video->height / 2, PACK_RGB(180, 180, 180));
    draw_rectangle(video, 0, video->height / 2, video->width, video->height/2, PACK_RGB(100, 100, 100));
    
    Texture map_tex = {0};
    map_tex.width = 100;
    map_tex.height = 100;
    map_tex.pixels = &state->pixels[0];
    map_tex.format = TEXTURE_RGBA;

    draw_rectangle(&map_tex, 0, 0, map_tex.width, map_tex.height, 0);
    draw_map(&map_tex, (const int *) map, state, tile_size);

    // draw player
    draw_rectangle(&map_tex, (int) (tile_size * state->playerX - 2.0f), (int) (tile_size * state->playerY - 2.0f), 4, 4, COLOR_GREEN);
    
    
    //real32 player_dir_x = cosf(deg_to_rad(state->player_view_angle));
    //real32 player_dir_y = sinf(deg_to_rad(state->player_view_angle));
    
    //const real32 fov = 30.0f;
    
    
    const real32 n = 0.1f;
    const real32 L = 0.05f;
    //const real32 W = (real32) video->width - 1;
    const real32 W = (real32) video->width;
    
    for (uint32 x = 0; x < video->width; ++x)
    {
        
        real32 player_dir_x = cosf(deg_to_rad(state->player_view_angle));
        real32 player_dir_y = sinf(deg_to_rad(state->player_view_angle));
        
        real32 side_x = -player_dir_y;
        real32 side_y = player_dir_x;
        
        //real32 view_plane_center_x = player_dir_x * n;
        //real32 view_plane_center_y = player_dir_y * n;
        
        //real32 temp1 = ((W * L) - (2 * x * L)) / W;
	real32 temp1 = (L * (W - 1 - 2*x)) / W;
        real32 pixel_vec_magnitude = sqrtf(n * n  + temp1 * temp1);
        
        real32 pixel_dir_x = (n * player_dir_x  + temp1 * side_x) / pixel_vec_magnitude;
        real32 pixel_dir_y = (n * player_dir_y  + temp1 * side_y) / pixel_vec_magnitude;
        
        real32 dir_x = pixel_dir_x;
        real32 dir_y = pixel_dir_y;
        
        int cell_x = (int) state->playerX;
        int cell_y = (int) state->playerY;
        
        real32 start_x = state->playerX;
        real32 start_y = state->playerY;
        real32 current_x = state->playerX;
        real32 current_y = state->playerY;
        
        bool32 hit = false;
        
        real32 t = 0;
        
        int inc_x = (dir_x > 0) ? 1 : -1;
        int inc_y = (dir_y > 0) ? 1 : -1;
        
        int fy = (dir_y > 0) ? 0 : 1;
        int fx = (dir_x > 0) ? 0 : 1;
        
        bool32 side = false;
        while (current_x <= 10 && current_x >= 0  && current_y <= 10 && current_y >= 0 && !hit)
        {
            //	    cell_x = (int) current_x;
            //	    cell_y = (int) current_y;
            
            int cell_value = ((int *) map)[(10 * (9 - cell_y)) + cell_x];
            if (cell_value == 1)
            {
                hit = true;
                break;
            }
            
            int next_cell_x = cell_x + inc_x;
            int next_cell_y = cell_y + inc_y;
            
            real32 dtx = (((real32)next_cell_x + fx) - current_x) / dir_x;
            real32 dty = (((real32)next_cell_y + fy) - current_y) / dir_y;
            
            if (dtx < dty)
            {
                cell_x = next_cell_x;
                t += dtx;
                side = true;
            }
            else
            {
                cell_y = next_cell_y;
                t += dty;
                side = false;
            }
            
            current_x = start_x + dir_x * t;
            current_y = start_y + dir_y * t;
        }
        
        if (!hit) {
	    DebugLog(platform, "No hit! %d", x);
	    continue;
	}
        
        
        //real32 z = t * n / sqrtf(n * n + temp1 * temp1);
	real32 z = ((player_dir_x * (current_x - start_x)) + (player_dir_y * (current_y - start_y)));
        real32 line_length = tile_size * t;
        draw_line(&map_tex,
                  (int) (tile_size * state->playerX),
                  (int) (tile_size * state->playerY),
                  (int) (line_length * dir_x + (tile_size * state->playerX)),
                  (int) (line_length * dir_y + (tile_size * state->playerY)),
                  PACK_RGB(0, 0, 255));
        
        
        real32 h0 = video->height * 0.4f;
        uint32 height = (uint32) (h0  / z);

	real32 tex_side = (current_y - cell_y);
	real32 tex_not_side = (current_x - cell_x);
        
        draw_column(video, x, height, side ? tex_side : 0.0f, 0.0f, side ? 0.0f : tex_not_side, side ? tex_side : tex_not_side, &test_bitmap_texture);
    }

    draw_texture(&map_tex, video, 0, 0);
}
