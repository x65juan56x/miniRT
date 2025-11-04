#include <float.h>
#include "../../include/minirt.h"
#include "../../include/scene.h"
#include "../../include/hit.h"

static float	hit_cap(const t_cyl *cyl, t_ray r, int sign)
{
	float	t;
	t_vec3	p;
	t_vec3	c_cap;
	t_vec3	radial;
	float	denom;

	denom = v3_dot(r.dir, cyl->axis);
	if (fabsf(denom) < 1e-6f)
		return (-1.0f);
	if (sign == 1)
		c_cap = cyl->vars.cap_top;
	else
		c_cap = cyl->vars.cap_bottom;
	t = v3_dot(v3_sub(c_cap, r.orig), cyl->axis) / denom;
	if (t < 0.0f)
		return (-1.0f);
	p = v3_add(r.orig, v3_mul(r.dir, t));
	radial = v3_sub(p, c_cap);
	if (v3_dot(radial, radial) <= cyl->vars.radius2)
		return (t);
	return (-1.0f);
}
/*
* Purpose: Find where a ray hits a cylinder cap (the flat circular ends).
* Inputs: cyl (cylinder geometry), ray r, sign (1 = top cap, -1 = bottom cap).
* Algorithm:
*   - First check if ray is flying parallel to the cap → will never hit
*   - Find where ray crosses the cap's plane
*   - Check if that point is actually inside the circular disk (within radius)
*   - Reject hits behind the ray origin (t < 0)
* Returns: distance to hit point, or -1 if no hit.
*/

static void	cyl_quadratic(t_cyl *cyl, t_ray r, t_vec3 x)
{
	float	x_dot_ax;
	float	d_dot_ax;

	x_dot_ax = v3_dot(x, cyl->axis);
	d_dot_ax = v3_dot(r.dir, cyl->axis);
	cyl->vars.a = v3_dot(v3_sub(r.dir, v3_mul(cyl->axis, d_dot_ax)),
			v3_sub(r.dir, v3_mul(cyl->axis, d_dot_ax)));
	cyl->vars.b = 2.0f * v3_dot(v3_sub(r.dir, v3_mul(cyl->axis, d_dot_ax)),
			v3_sub(x, v3_mul(cyl->axis, x_dot_ax)));
	cyl->vars.c = (v3_dot(v3_sub(x, v3_mul(cyl->axis, x_dot_ax)),
				v3_sub(x, v3_mul(cyl->axis, x_dot_ax)))) - cyl->vars.radius2;
}
/*
* Purpose: Set up the quadratic equation to find where ray hits the curved side.
* Inputs: cyl (cylinder), ray r, x = vector from cylinder center to ray origin.
* Algorithm:
*   - We project everything onto the plane perpendicular to the cylinder's axis
*   - This simplifies the 3D problem: "does the ray hit the curved surface?"
*   - Builds coefficients a, b, c for the equation a*t² + b*t + c = 0
*   - These values get stored in cyl->vars for later solving
* Use: Called by hit_side() to prepare for intersection calculation.
*/

static float	hit_side(t_cyl *cyl, t_ray r)
{
	float	disc;
	float	t1;
	float	t2;
	float	tmp;

	cyl_quadratic(cyl, r, v3_sub(r.orig, cyl->center));
	if (cyl->vars.a == 0.0f)
		return (-1.0f);
	disc = (cyl->vars.b * cyl->vars.b) - (4 * cyl->vars.a * cyl->vars.c);
	if (disc < 0.0f)
		return (-1.0f);
	t1 = (-cyl->vars.b - sqrtf(disc)) / (2.0f * cyl->vars.a);
	t2 = (-cyl->vars.b + sqrtf(disc)) / (2.0f * cyl->vars.a);
	if (t1 > t2)
	{
		tmp = t1;
		t1 = t2;
		t2 = tmp;
	}
	return (pick_valid_t(cyl, r, t1, t2));
}
/*
* Purpose: Find where a ray hits the cylinder's curved side (not the caps).
* Algorithm:
*   - Set up quadratic equation using cyl_quadratic()
*   - If a == 0, ray runs parallel to the cylinder → no side hit
*   - disc (discriminant): tells us if ray hits the curved surface
*       • If negative → ray completely misses
*       • If zero or positive → ray hits (possibly twice: entering and exiting)
*   - Calculate the two possible hit distances (t1, t2)
*   - Use pick_valid_t() to choose the closest hit that's within the cylinder's
*     height (between the two caps) and in front of us
* Returns: distance to hit point, or -1 if no hit.
*/

float	hit_cylinder(t_cyl *cyl, t_ray r)
{
	float	best_t;
	float	t_side;
	float	t_top;
	float	t_bottom;

	best_t = FLT_MAX;
	t_side = hit_side(cyl, r);
	cyl->vars.hit_part = -1;
	best_t = check_best_t(t_side, best_t, cyl, 0);
	t_top = hit_cap(cyl, r, 1);
	best_t = check_best_t(t_top, best_t, cyl, 1);
	t_bottom = hit_cap(cyl, r, -1);
	best_t = check_best_t(t_bottom, best_t, cyl, 2);
	if (best_t >= FLT_MAX || cyl->vars.hit_part == -1)
		return (-1.0f);
	return (best_t);
}
/*
* Purpose: Find the nearest point where a ray hits any part of the cylinder.
* Algorithm:
*   - Test three possible surfaces: curved side, top cap, bottom cap
*   - Keep track of which surface gives the closest hit (smallest t > 0)
*   - Record which part was hit in cyl->vars.hit_part:
*       • 0 = curved side
*       • 1 = top cap
*       • 2 = bottom cap
*   - This info is used later to calculate the correct surface normal
* Returns: distance to nearest hit, or -1 if ray misses all parts.
* Use: Main entry point for all ray-cylinder intersection tests.
*/

/* float	hit_cylinder(t_cyl *cyl, t_ray r)
{
	float	best_t;
	float	t_side;
	float	t_top;
	float	t_bottom;

	best_t = FLT_MAX;
	t_side = hit_side(cyl, r);
	cyl->vars.hit_part = -1;
	if(t_side > 0.0f && t_side < best_t)
	{
		best_t = t_side;
		cyl->vars.hit_part = 0;
	}
	t_top = hit_cap(cyl, r, 1);
	if(t_top > 0.0f && t_top < best_t)
	{
		best_t = t_top;
		cyl->vars.hit_part = 1;
	}
	t_bottom = hit_cap(cyl, r, -1);
	if(t_bottom > 0.0f && t_bottom < best_t)
	{
		best_t = t_bottom;
		cyl->vars.hit_part = 2;
	}
	if (best_t >= FLT_MAX || cyl->vars.hit_part == -1)
        return -1.0f;
	return best_t;
} */
