#include "../../include/ray.h"

t_ray	ray(t_vec3 o, t_vec3 d)
{
	t_ray	r;

	r.orig = o;
	r.dir = d;
	return (r);
}
/*
* Ray constructor: defines a ray by its origin `o` and its direction `d`.
* In ray tracing, `o` is usually the position of the camera or an intersection
* point, and `d` is a direction vector (not necessarily normalised)
* towards which the ray travels.
*/

t_vec3	ray_at(t_ray r, float t)
{
	return (v3_add(r.orig, v3_mul(r.dir, t)));
}
/*
* Evaluates a point along the ray: P(t) = orig + t * dir.
* `t` is a parametric distance/time (t >= 0 moves in the direction of the ray).
* This is used to calculate the exact point of intersection once `t` is known.
*/
