#include <float.h>
#include "../../include_bonus/minirt_bonus.h"

static int	inside_cyl_height(const t_cyl *cylinder, t_vec3 p, t_vec3 v)
{
	float	height_pos;

	height_pos = v3_dot(v3_sub(p, cylinder->center), v);
	if ((fabsf(height_pos) <= cylinder->vars.half_height))
		return (1);
	return (0);
}
/*
* Purpose: Check if a point is between the cylinder's top and bottom caps.
* Inputs: cylinder, point p, axis vector v.
* Algorithm:
*   - Project the point onto the cylinder's axis (measure how far along the axis)
*   - If the distance from center is <= half_height, it's between the caps
* Returns: 1 if within height bounds, 0 if outside.
* Use: After finding where ray hits the curved side, verify it's not above/below caps.
*/

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

static float	pick_valid_t(const t_cyl *cyl, t_ray r, float t1, float t2)
{
	float	tmin;
	float	tmax;
	t_vec3	p;

	tmin = t2;
	if (t1 > 0.0f)
		tmin = t1;
	tmax = t1;
	if (t2 > 0.0f)
		tmax = t2;
	if (tmin > 0.0f)
	{
		p = v3_add(r.orig, v3_mul(r.dir, tmin));
		if (inside_cyl_height(cyl, p, cyl->axis))
			return (tmin);
	}
	if (tmax > 0.0f && tmax >= tmin)
	{
		p = v3_add(r.orig, v3_mul(r.dir, tmax));
		if (inside_cyl_height(cyl, p, cyl->axis))
			return (tmax);
	}
	return (-1.0f);
}
/*
* Purpose: Choose the best hit point from two possible solutions.
* Inputs: cyl (for height checking), ray r, t1 and t2 (two possible distances).
* Algorithm:
*   - We might have two points where ray crosses the curved surface
*   - Try the closer one (tmin) first: check if it's in front of us (t > 0)
*     and between the caps (inside height bounds)
*   - If that fails, try the farther one (tmax)
*   - Prefer closer hits when both are valid
* Returns: distance to valid hit, or -1 if both fail the checks.
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

/*
float	hit_cylinder(t_cyl *cyl, t_ray r)
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
}
*/
