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
* Purpose: Check if a point lies within the cylinder's height bounds.
* Inputs: cylinder (with center, axis, half_height), point p, axis vector v.
* Logic: Project (p - center) onto the axis; if |projection| <= half_height,
*        the point is between the two caps.
* Returns: 1 if within bounds, 0 otherwise.
* Use: Validates that ray-cylinder side intersections fall within the finite
*      capped cylinder.
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
		return (-1.0f); //paralelo no hay interseccion
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
* Purpose: Test ray intersection with a cylinder cap (circular disk).
* Inputs: cyl (axis, cap centers, radius), ray r, sign (1=top, -1=bottom).
* Algorithm:
*   1. Check if ray is parallel to cap plane (dot(r.dir, axis) ≈ 0) -> no hit.
*   2. Solve plane intersection: t = dot(cap_center - r.orig, axis) / denom.
*   3. If t < 0, intersection is behind ray origin -> reject.
*   4. Compute hit point p and check if distance from cap center <= radius.
* Returns: t >= 0 for valid intersection, -1.0f otherwise.
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
* Purpose: Pick the nearest valid root from the quadratic solutions for the
*          cylinder side intersection.
* Inputs: cyl (height limits, axis), ray r, roots t1 and t2 (may be negative).
* Logic: Order roots (tmin = nearest positive), test each against height bounds
*        using inside_cyl_height(). Return first valid root, or -1 if none.
* Returns: t >= 0 for valid side hit within height bounds, -1.0f otherwise.
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
* Purpose: Compute ray intersection with the finite cylinder's curved side.
* Algorithm:
*   1. Compute quadratic coefficients via cyl_quadratic() (projects ray onto
*      plane perpendicular to axis).
*   2. If a == 0, ray is parallel to axis after projection -> no side hit.
*   3. Compute discriminant; if negative, no real roots -> no intersection.
*   4. Solve for roots t1, t2, order them, and call pick_valid_t() to find
*      the nearest positive root within height bounds.
* Returns: t >= 0 for valid side hit, -1.0f otherwise.
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
* Purpose: Find the nearest intersection between a ray and a finite cylinder
*          (side and both caps).
* Algorithm:
*   1. Test curved side with hit_side().
*   2. Test top cap (sign=1) and bottom cap (sign=-1) with hit_cap().
*   3. Use check_best_t() to track the smallest t and record which part was hit
*      in cyl->vars.hit_part (0=side, 1=top, 2=bottom) for normal computation.
* Returns: smallest t >= 0 for a valid hit, -1.0f if ray misses entirely.
* Use: Main entry point for ray-cylinder intersection tests.
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
