#include <math.h>
#include "../../include/vec3.h"

t_vec3	v3_ctoc(t_vec3 a, t_vec3 b)
{
	return (v3(
			a.x * b.x,
			a.y * b.y,
			a.z * b.z));
}
/*
* Component to component product
*/

t_vec3	v3_cross(t_vec3 a, t_vec3 b)
{
	return (v3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x));
}
/*
* Cross product: perpendicular to both vectors.
* Returns a vector whose direction follows the right-hand rule and whose
	magnitude is |a||b|sin(theta).
* Useful for constructing orthonormal bases.
*/

float	v3_len2(t_vec3 a)
{
	return (v3_dot(a, a));
}
/*
* Length squared: avoid sqrt when you only need to compare magnitudes.
*/

float	v3_len(t_vec3 a)
{
	return (sqrtf(v3_len2(a)));
}
/*
* Length (Euclidean norm) of the vector: sqrt(a·a).
* Useful to obtain the physical magnitude of a vector (distance, speed, etc.).
*/

t_vec3	v3_norm(t_vec3 a)
{
	float	l;

	l = v3_len(a);
	if (l > 0.0f)
		return (v3_div(a, l));
	else
		return (v3(0.0f, 0.0f, 0.0f));
}
/*
* Normalisation: returns a vector with the same direction but length 1.
* If the length is 0, returns (0,0,0) to avoid division by zero.
*/
