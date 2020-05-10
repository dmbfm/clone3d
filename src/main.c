#include <stdio.h>
#include "main.h"
#include "math.h"

typedef struct {
    real32 r;
    real32 g;
    real32 b;
} Color;

int clamp_i(int x, int a, int b)
{
    if (x < a)
    {
	return a;
    }

    if (x > b)
    {
	return b;
    }

    return x;
}

bool32 video_check_pixel_coords(Video *video, int x, int y)
{
    return ((x >= 0) && (x < ((int) video->width)) && (y >= 0) && (y < ((int) video->height)));
}

void draw_rectangle(Video *video, int x, int y, int width, int height, real32 r, real32 g, real32 b)
{
    if (!video_check_pixel_coords(video, x, y))
    {
	return;
    }	   

    int ymax = clamp_i(y + height, y, video->height);
    int xmax = clamp_i(x + width, x, video->width);

    for (int j = y; j < ymax; ++j)
    {
	uint32 *row = &(video->framebuffer[j * video->width]);
	for (int i = x; i < xmax; ++i)
	{
	    uint32 *pixel = &row[i];
	    uint8 _r = (uint8) (r * 255.0f);
	    uint8 _g = (uint8) (g * 255.0f);
	    uint8 _b = (uint8) (b * 255.0f);
	    uint8 a = 255;
	    
	    *pixel = (a << 24) | (_b << 16) | (_g << 8) | _r;
	}
    }
}

void draw_column(Video *video, uint32 x, uint32 height, real32 r, real32 g, real32 b)
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

    for (uint32 j = y0; j < ymax; ++j)
    {
	uint32 *pixel = &(video->framebuffer[j * video->width + x]);
	
	uint8 _r = (uint8) (r * 255.0f);
	uint8 _g = (uint8) (g * 255.0f);
	uint8 _b = (uint8) (b * 255.0f);
	uint8 a = 255;
	    
	*pixel = (a << 24) | (_b << 16) | (_g << 8) | _r;
    }
}


typedef struct {
    int offsetX;
    int offsetY;

    real32 playerX;
    real32 playerY;
    // TODO: make this real32, or, make that we go from 0 to 36000 or something, instead
    // of 0, 360
    int16 player_view_angle;
    
} State;

global State *state;


#define DebugLog(P, STR, ...)				\
    { char buf[256]; snprintf(buf, 256, STR, __VA_ARGS__); P->print_string(buf); }

#define M_PI 3.141592653589f

inline int is_zero(real32 x, real32 eps) {
    if (eps < 0)
    {
	eps *= -1.0f;
    }
    
    return ((x >= -eps) && (x <= eps));
}

inline real32 deg_to_rad(real32 deg) {
    return (deg * (M_PI / 180.0f));
}

void draw_map(Video *video, const int *map, State *state, int size)
{
    for (int i = 0; i < 10; ++i)
    {
	for (int j = 0; j < 10; ++j)
	{
	    int value = map[10 * (9 - j) + i];

	    real32 r = 1.0f;
	    real32 g = value ? 0.0f : 1.0f;
	    real32 b = value ? 0.0f : 1.0f;
	    
	    draw_rectangle(video, size * i + 1, size * j+ 1, size - 1, size - 1, r, g, b);
	}
    }
}

void video_put_pixel(Video *video, int x, int y, Color *color)
{
    if ((x < 0) || (x >= (int)video->width) || (y < 0) || (y >= (int)video->height))
    {
	return;
    }
    
    
    uint32 *pixel = &video->framebuffer[y * video->width + x];

    uint8 _r = (uint8) (color->r * 255.0f);
    uint8 _g = (uint8) (color->g * 255.0f);
    uint8 _b = (uint8) (color->b * 255.0f);
    uint8 a = 255;
	    
    *pixel = (a << 24) | (_b << 16) | (_g << 8) | _r;
}

void draw_line_octant_0(Video *video, uint32 x0, uint32 y0, uint32 dx, uint32 dy, int x_direction, Color *color)
{

    int dy_x_2 = 2 * dy;
    int dy_minus_dx_x_2 = dy_x_2 - (int) (2 * dx);
    int error_term = dy_x_2 - ((int) dx);
    
    int x = x0;
    int y = y0;
    video_put_pixel(video, x, y, color);
    while(dx--)
    {
	if (error_term >= 0)
	{
	    y++;
	    error_term += dy_minus_dx_x_2;
	}
	else
	{
	    error_term += dy_x_2;
	}

	x += x_direction;
	video_put_pixel(video, x, y, color);
    }
}

void draw_line_octant_1(Video *video, uint32 x0, uint32 y0, uint32 dx, uint32 dy, int x_direction, Color *color)
{

    int dy_x_2 = 2 * dx;
    int dy_minus_dx_x_2 = 2 * (dx - dy);    
    int error_term = dy_x_2 - dy;
    
    int x = x0;
    int y = y0;

    video_put_pixel(video, x, y, color);
    while(dy--)
    {
	if (error_term > 0)
	{
	    x += x_direction;
	    error_term += dy_minus_dx_x_2;
	}
	else
	{
	    error_term += dy_x_2;
	}

	++y;
	video_put_pixel(video, x, y, color);
    }
}

void draw_line(Video *video, int x0, int y0, int x, int y, Color *color)
{
    // Switch (x0, y0) with (x, y) to make sure dy > 0.
    if (y < y0)
    {
	int temp = x;
	x = x0;
	x0 = temp;

	temp = y;
	y = y0;
	y0 = temp;
    }

    int dx = x - x0;
    int dy = y - y0;

    if (dx > 0)
    {
	if (dx > dy)
	{
	    draw_line_octant_0(video, x0, y0, dx, dy, 1, color);
	}
	else
	{
	    draw_line_octant_1(video, x0, y0, dx, dy, 1, color);
	}
    }
    else
    {
	dx = - dx;
	if (dx > dy)
	{
	    draw_line_octant_0(video, x0, y0, dx, dy, -1, color);
	}
	else
	{
	    draw_line_octant_1(video, x0, y0, dx, dy, -1, color);
	}
    }
}

#define CELL(map, i, j) (map[10 * j + i])


uint32 TEST_raycast(Video *video, PlatformLayer *platform, State *state, int *map)
{
    
       
    return 10;
}

void Update(Video *video, PlatformLayer *platform, Memory *memory, InputState *input) {

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

    if (input->key_up.is_down)
    {
	++state->offsetY;

	real32 fx = cosf(deg_to_rad(state->player_view_angle));
	real32 fy = sinf(deg_to_rad(state->player_view_angle));

	real32 pX = state->playerX;
	real32 pY = state->playerY;
	
	state->playerX += 0.01f * fx;
	state->playerY += 0.01f * fy;
	
	/* int cell_x = (int) state->playerX; */
	/* int cell_y = (int) state->playerY; */

	/* int cell_value = ((int *) map)[(10 * (9 - cell_y)) + cell_x]; */
	
	/* if (cell_value) */
	/* { */
	/*     state->playerX = pX; */
	/*     state->playerY = pY; */
	/* } */

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
    
    draw_rectangle(video, 0, 0, video->width, video->height, 0, 0, 0);
    draw_rectangle(video, 0, 0, video->width, video->height / 2, 0.8f, 0.8f, 0.8f);
    draw_rectangle(video, 0, video->height / 2, video->width, video->height/2, 0.4f, 0.4f, 0.4f);

    

    
    draw_map(video, (const int *) map, state, tile_size);
    // draw player
    draw_rectangle(video, tile_size * state->playerX - 2.0f, tile_size * state->playerY - 2.0f, 4, 4, 0.0f, 1.0f, 0.0f);
    

    real32 player_dir_x = cosf(deg_to_rad(state->player_view_angle));
    real32 player_dir_y = sinf(deg_to_rad(state->player_view_angle));

    const real32 fov = 30.0f;


    const real32 n = 0.1f;
    const real32 L = 0.05f;
    const real32 W = (real32) video->width - 1;
    
    for (int x = 0; x < video->width; ++x)
    {

	real32 player_dir_x = cosf(deg_to_rad(state->player_view_angle));
	real32 player_dir_y = sinf(deg_to_rad(state->player_view_angle));

	real32 side_x = -player_dir_y;
	real32 side_y = player_dir_x;

	real32 view_plane_center_x = player_dir_x * n;
	real32 view_plane_center_y = player_dir_y * n;

	real32 temp1 = ((W * L) - (2 * x * L)) / W;
	real32 pixel_vec_magnitude = sqrtf(n * n  + temp1 * temp1);

	real32 pixel_dir_x = (n * player_dir_x  + temp1 * side_x) / pixel_vec_magnitude;
	real32 pixel_dir_y = (n * player_dir_y  + temp1 * side_y) / pixel_vec_magnitude;
	
//	real32 angle = fov - (((2 * fov) / video->width) * x);
	
//	real32 dir_x = cosf(deg_to_rad(state->player_view_angle + angle));
//	real32 dir_y = sinf(deg_to_rad(state->player_view_angle + angle));

	real32 dir_x = pixel_dir_x;
	real32 dir_y = pixel_dir_y;
    

	// RAYCAST TEST

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

	if (!hit) DebugLog(platform, "No hit!");

	

//	real32 z = (t * cosf(deg_to_rad(angle)));
	real32 z = t * n / sqrtf(n * n + temp1 * temp1);

	//if (z < n) DebugLog(platform, "Z < n\n  !!");

	real32 line_length = tile_size * t;
	Color line_color =  { 0.0f, 0.0f, 1.0f };
	draw_line(video,
		  tile_size * state->playerX,
		  tile_size * state->playerY,
		  line_length * dir_x + (tile_size * state->playerX),
		  line_length * dir_y + (tile_size * state->playerY),
		  &line_color);
	
	//draw_rectangle(video, tile_size * cell_x - 2.0f, tile_size * cell_y - 2.0f, 4, 4, 0.0f, 1.0f, 0.0f);

	real32 h0 = video->height * 0.4f;
	uint32 height = (uint32) (h0  / z);

	draw_column(video, x, height, side ? 1.0f : 0.0f, 0.0f, side ? 0.0f : 1.0f);
    }
}
