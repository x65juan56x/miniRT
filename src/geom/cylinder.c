#include <float.h>
#include "../../include/minirt.h"
#include "../../include/scene.h"
#include "../../include/hit.h"

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
* Logic: Project (p - center) onto the axis; compare magnitude to half_height.
* Use: Called to validate if a ray-cylinder intersection is within the
	finite height.
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
* Purpose: Test ray intersection with the top or bottom cap of the cylinder.
* Logic: Treat cap as a disk; compute plane intersection, check radius.
* Notes: sign=1 for top cap, sign=-1 for bottom; returns t if inside disk.
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
* Purpose: Compute quadratic equation coefficients for ray-cylinder intersection.
* Logic: Project ray and origin onto the plane perpendicular to the axis.
* Notes: Sets a, b, c in cyl->vars for solving the quadratic;
	x = r.orig - center.
*/

static float	pick_valid_t(const t_cyl *cyl, t_ray r, float t1, float t2)
{
	float	tmin;
	float	tmax;
	t_vec3	p;

	tmin = t2;
	if (t1 > EPSILON)
		tmin = t1;
	tmax = t1;
	if (t2 > EPSILON)
		tmax = t2;
	if (tmin > EPSILON)
	{
		p = v3_add(r.orig, v3_mul(r.dir, tmin));
		if (inside_cyl_height(cyl, p, cyl->axis))
			return (tmin);
	}
	if (tmax > EPSILON && tmax >= tmin)
	{
		p = v3_add(r.orig, v3_mul(r.dir, tmax));
		if (inside_cyl_height(cyl, p, cyl->axis))
			return (tmax);
	}
	return (-1.0f);
}
/*
* Purpose: Select the nearest valid intersection on the cylinder's
	curved surface.
* Logic: Check both roots of the quadratic; return the first within
	height bounds.
* Notes: Prefers tmin (closer root); validates height with inside_cyl_height.
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
* Purpose: Compute ray intersection with the infinite cylinder's curved surface.
* Logic: Solve quadratic equation; check discriminant and pick valid t.
* Notes: Returns -1 if no intersection or if both roots are invalid.
*/

static float	check_best_t(float cyl_part, float best_t, t_cyl *cyl,
		int index)
{
	if (cyl_part > 0.0f && cyl_part < best_t)
	{
		cyl->vars.hit_part = index;
		return (cyl_part);
	}
	return (best_t);
}

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
* Purpose: Find the nearest ray-cylinder intersection (side or caps).
* Logic: Test side, top cap, and bottom cap; return the closest valid hit.
* Notes: Sets cyl->vars.hit_part (0=side, 1=top, 2=bottom) for normal
	calculation.
* Use: Called by the intersection dispatcher; returns t or -1 if no hit.
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
