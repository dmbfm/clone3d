#include "graphics.h"

void draw_texture(Texture *src, Texture *dst, int x0, int y0)
{
    if (src->format == TEXTURE_RGB)
    {
	for (uint32 y = 0; y < src->height; ++y)
	{
	    uint32 *row = &src->pixels[y * src->width];
	    for (uint32 x = 0; x < src->width; ++x)
	    {
		uint32 *pixel = &row[x];
		put_pixel(dst, x + x0, y + y0, *pixel);
	    }
	}
    }
    else
    {
	for (uint32 y = 0; y < src->height; ++y)
	{
	    //uint32 *row = &src->pixels[y * src->width];
	    for (uint32 x = 0; x < src->width; ++x)
	    {
		//uint32 *pixel = &row[x];
		uint8 src_r, src_g, src_b, src_a;
		texture_get_rgba(src, x, y, &src_r, &src_g, &src_b, &src_a);

		uint8 dst_r, dst_g, dst_b, dst_a;
		texture_get_rgba(dst, x+x0, y+y0, &dst_r, &dst_g, &dst_b, &dst_a);

		real32 src_alpha = (real32) (src_a / 255.0f);
		real32 one_minus_src_alpha = 1 - src_alpha;

		uint8 r = (uint8) (src_r * src_alpha + dst_r * one_minus_src_alpha);
		uint8 g = (uint8) (src_g * src_alpha + dst_g * one_minus_src_alpha);
		uint8 b = (uint8) (src_b * src_alpha + dst_b * one_minus_src_alpha);
		
		put_pixel(dst, x + x0, y + y0, PACK_RGB(r, g, b));
	    }
	}
    }
}

void put_pixel(Texture *tex, int x, int y, uint32 color)
{
    if ((x < 0) || (x >= (int)tex->width) || (y < 0) || (y >= (int)tex->height))
    {
        return;
    }
    
    
    uint32 *pixel = &tex->pixels[y * tex->width + x];
    
    *pixel = color; // ((color->a) << 24) | ((color->b) << 16) | ((color->g) << 8) | (color->r);
}

void texture_get_pixel(Texture *tex, int x, int y, uint32 *color)
{
    *color = tex->pixels[y * tex->width + x];
}

void texture_get_rgba(Texture *tex, int x, int y, uint8 *r, uint8 *g, uint8 *b, uint8 *a)
{
    uint8 *pixel = (uint8 *) (&tex->pixels[y * tex->width + x]);

    *r = *(pixel++);
    *g = *(pixel++);
    *b = *(pixel++);
    *a = *pixel;
}


inline bool32 check_pixel_coords(Texture *tex, int x, int y)
{
    return ((x >= 0) && (x < ((int) tex->width)) && (y >= 0) && (y < ((int) tex->height)));
}
void draw_rectangle(Texture *tex, int x, int y, int width, int height, uint32 color)
{
    if (!check_pixel_coords(tex, x, y))
    {
        return;
    }	   
    
    int ymax = clamp_i(y + height - 1, y, tex->height);
    int xmax = clamp_i(x + width - 1, x, tex->width);
    
    for (int j = y; j < ymax; ++j)
    {
        uint32 *row = &(tex->pixels[j * tex->width]);
        for (int i = x; i < xmax; ++i)
        {
            uint32 *pixel = &row[i];
            
            *pixel = color;
        }
    }
}

void draw_line_octant_0(Texture *tex, uint32 x0, uint32 y0, uint32 dx, uint32 dy, int x_direction, uint32 color)
{
    
    int dy_x_2 = 2 * dy;
    int dy_minus_dx_x_2 = dy_x_2 - (int) (2 * dx);
    int error_term = dy_x_2 - ((int) dx);
    
    int x = x0;
    int y = y0;
    put_pixel(tex, x, y, color);
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
        put_pixel(tex, x, y, color);
    }
}

void draw_line_octant_1(Texture *tex, uint32 x0, uint32 y0, uint32 dx, uint32 dy, int x_direction, uint32 color)
{
    
    int dy_x_2 = 2 * dx;
    int dy_minus_dx_x_2 = 2 * (dx - dy);    
    int error_term = dy_x_2 - dy;
    
    int x = x0;
    int y = y0;
    
    put_pixel(tex, x, y, color);
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
        put_pixel(tex, x, y, color);
    }
}

void draw_line(Texture *tex, int x0, int y0, int x, int y, uint32 color)
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
            draw_line_octant_0(tex, x0, y0, dx, dy, 1, color);
        }
        else
        {
            draw_line_octant_1(tex, x0, y0, dx, dy, 1, color);
        }
    }
    else
    {
        dx = - dx;
        if (dx > dy)
        {
            draw_line_octant_0(tex, x0, y0, dx, dy, -1, color);
        }
        else
        {
            draw_line_octant_1(tex, x0, y0, dx, dy, -1, color);
        }
    }
}
