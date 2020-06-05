#if !defined(MATH_H)
#define MATH_H

#define M_PI 3.141592653589f

int clamp_i(int x, int a, int b);
int32 round32(real32 x);
int32 ceil32(real32 x);
int32 floor32(real32 x);

inline int is_zero(real32 x, real32 eps);
inline real32 deg_to_rad(real32 deg);

#endif
