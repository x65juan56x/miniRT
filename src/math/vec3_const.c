#include <math.h>
#include "../../include/vec3.h"

float	v3_dot(t_vec3 a, t_vec3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}
/*
* Scalar product. Measures how much ‘projection’ one vector has onto another.
* Useful for angles and shading.
*/