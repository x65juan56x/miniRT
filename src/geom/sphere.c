#include "../../include/minirt.h"
#include "../../include/scene.h"
#include "../../include/hit.h"

float	hit_sphere(t_sphere *sp, t_ray r)
{
	float		t;

	sp->vars.oc = v3_sub(r.orig, sp->center);
	sp->vars.a = v3_dot(r.dir, r.dir);
	sp->vars.half_b = v3_dot(sp->vars.oc, r.dir);
	sp->vars.c = v3_dot(sp->vars.oc, sp->vars.oc) - sp->vars.radius2;
	sp->vars.disc = sp->vars.half_b * sp->vars.half_b - sp->vars.a * sp->vars.c;
	if (sp->vars.disc < 0.0f)
		return (-1.0f);
	t = (-sp->vars.half_b - sqrtf(sp->vars.disc)) / sp->vars.a;
	if (t > 0.0f)
		return (t);
	t = (-sp->vars.half_b + sqrtf(sp->vars.disc)) / sp->vars.a;
	if (t > 0.0f)
		return (t);
	return (-1.0f);
}
/*
* Purpose: Find where a ray hits a sphere (if at all).
* Inputs: sp (sphere with center and radius), ray r (origin and direction).
* Algorithm:
*   - We're looking for the distance 't' along the ray where it
		touches the sphere
*   - oc: vector pointing from sphere center to where the ray starts
*   - We solve a quadratic equation (like ax² + bx + c = 0) to find two possible
*     hit points: one when entering the sphere, one when exiting
*   - disc (discriminant): tells us if the ray hits at all
*       • If negative → ray completely misses the sphere
*       • If zero or positive → ray hits the sphere
*   - We prefer the closer hit point (entering the sphere)
*   - If both hits are behind us (t < 0), there's no visible intersection
* Returns: distance to hit point, or -1 if no hit.
*/
