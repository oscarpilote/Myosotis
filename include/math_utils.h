#ifndef _MATH_UTILS_H
#define _MATH_UTILS_H


#define PI 		3.14159265358979323846
#define SQRT2 		1.41421356237309504880
#define ONETHIRD	0.33333333333333333333

#define POW2(x) ((x) * (x))
#define POW3(x) ((x) * (x) * (x))

#define MAX(a,b) ((b) > (a) ? (b) : (a))
#define MIN(a,b) ((b) < (a) ? (b) : (a))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#define DEG_TO_RAD		(PI / 180)
#define RAD_TO_DEG		(180 / PI)

template <typename T>
bool approx_equal(T a, T b)
{
	/* All but two significand digits should match */ 
	T scal = 0.01f;
	return (scal * a - scal * b) == 0;
}

#endif /* _MATH_UTILS_H */
