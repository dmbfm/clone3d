#include "math.h"

int32 floor32(real32 x)
{
    int32 i = (int32) x;

    if (i > x)
    {
	return i-1;
    }

    return i;
}

int32 ceil32(real32 x)
{
    int32 i = (int32) x;

    if (i < x)
    {
	return i+1;
    }

    return i;
}

int32 round32(real32 x)
{
    if (x < 0)
    {
	return (int) (x - 0.5f);
    }

    return (int) (x + 0.5);
}

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
