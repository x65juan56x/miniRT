#include <math.h>
#include "../../include/math_utils.h"

float	deg2rad(float d)
{
	return (d * (float)M_PI / 180.0f);
}

float	clampf(float value, float min, float max)
{
	if (value < min)
		return (min);
	if (value > max)
		return (max);
	return (value);
}
/*
Purpose: Clamp a floating-point value to the range [min, max].
Logic:
- If the value is less than min, return min.
- If the value is greater than max, return max.
- If it’s within the range, return the original value.
*/

/*TEST IF THIS SQUARE ROOT FUNCTION RUNS FASTER*/
// float q_rsqrt(float number)
// {
// 	long		i;
// 	float		x2;
// 	float		y;
// 	const float	threehalfs = 1.5f;

// 	x2 = number * 0.5f;
// 	y = number;
// 	i = *(long *)&y;
// 	i = 0x5f3759df - (i >> 1);
// 	y = *(float *)&i;
// 	y = y * (threehalfs - (x2 * y * y));
// 	y = y * (threehalfs - (x2 * y * y));

// 	return (y);
// }