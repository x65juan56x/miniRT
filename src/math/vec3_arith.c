#include <math.h>
#include "../../include/vec3.h"

t_vec3	v3(float x, float y, float z)
{
	return ((t_vec3){x, y, z});
}
/*
* Creates a 3D vector with components x, y, z.
* Useful to initialize positions, directions and colors in the 3D space.
*/

t_vec3	v3_add(t_vec3 a, t_vec3 b)
{
	return (v3(a.x + b.x, a.y + b.y, a.z + b.z));
}
/*
* Useful to combine displacements or acumulate forces/colors.
*/

t_vec3	v3_sub(t_vec3 a, t_vec3 b)
{
	return (v3(a.x - b.x, a.y - b.y, a.z - b.z));
}
/*
* Useful to obtain direction vectors between two points: b->a = a - b.
*/

t_vec3	v3_mul(t_vec3 a, float s)
{
	return (v3(a.x * s, a.y * s, a.z * s));
}
/*
* Useful for changing the vector's lenght or interpolation.
*/

t_vec3	v3_div(t_vec3 a, float s)
{
	return (v3(a.x / s, a.y / s, a.z / s));
}
/*
* Useful for normalising or converting units.
*/
