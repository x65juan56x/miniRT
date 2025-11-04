#include "../../include/minirt.h"
#include "../../include/scene.h"
#include "../../include/hit.h"

float	hit_plane(t_plane *pl, t_ray r)
{
	float	den;
	float	t;

	den = v3_dot(pl->normal, r.dir);
	if (fabsf(den) < 1e-6f)
		return (-1.0f);
	t = (pl->vars.d - v3_dot(pl->normal, r.orig)) / den;
	if (t > 0.0f)
		return (t);
	return (-1.0f);
}
/*
* Purpose: Find where a ray hits an infinite flat plane.
* Inputs: pl (plane with normal and distance d), ray r (origin and direction).
* Algorithm:
*   - den: measures how much the ray is "pointing toward" the plane
*       • If den ≈ 0, the ray is parallel to the plane → will never hit
*   - Calculate distance 't' where the ray crosses the plane surface
*   - If t > 0, the hit is in front of us (visible)
*   - If t < 0, the hit is behind us (not visible)
* Returns: distance to hit point, or -1 if no hit (parallel or behind).
*/
