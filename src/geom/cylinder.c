#include <float.h>
#include "../../include/minirt.h"
#include "../../include/scene.h"
#include "../../include/hit.h"


static int inside_cyl_height(const t_cyl *cylinder, t_vec3 p, t_vec3 v)
{
	float height_pos = v3_dot(v3_sub(p, cylinder->center), v);
	if ((fabsf(height_pos) <= cylinder->vars.half_height))
		return (1);
	return (0);
}

static float hit_cap(const t_cyl *cyl, t_ray r, int sign)
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
	if(t < 0.0f)
		return (-1.0f);
	p = v3_add(r.orig, v3_mul(r.dir, t));
	radial = v3_sub(p, c_cap);
	if(v3_dot(radial, radial) <= cyl->vars.radius2)
		return (t);
	return (-1.0f);
}

static void cyl_quadratic(t_cyl *cyl, t_ray r, t_vec3 x)
{
	float x_dot_ax;
	float d_dot_ax;

	x_dot_ax = v3_dot(x, cyl->axis);
	d_dot_ax = v3_dot(r.dir, cyl->axis);
	cyl->vars.a = v3_dot(v3_sub(r.dir, v3_mul(cyl->axis, d_dot_ax)),
		v3_sub(r.dir, v3_mul(cyl->axis, d_dot_ax)));
	cyl->vars.b = 2.0f* v3_dot(v3_sub(r.dir, v3_mul(cyl->axis, d_dot_ax)),
		v3_sub(x, v3_mul(cyl->axis, x_dot_ax)));
	cyl->vars.c = (v3_dot(v3_sub(x, v3_mul(cyl->axis, x_dot_ax)),
		v3_sub(x, v3_mul(cyl->axis, x_dot_ax)))) - cyl->vars.radius2;
}

static float pick_valid_t(const t_cyl *cyl, t_ray r, float t1, float t2)
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
	if(tmin > EPSILON)
	{
		p = v3_add(r.orig, v3_mul(r.dir, tmin));
		if (inside_cyl_height(cyl, p, cyl->axis))
			return (tmin);
	}
	if(tmax > EPSILON && tmax >= tmin)
	{
		p = v3_add(r.orig, v3_mul(r.dir, tmax));
		if (inside_cyl_height(cyl, p, cyl->axis))
			return (tmax);
	}
	return (-1.0f);
}

static float hit_side(t_cyl *cyl, t_ray r)
{
	float	disc;
	float	t1;
	float	t2;
	float	tmp;

	cyl_quadratic(cyl, r, v3_sub(r.orig, cyl->center));
	if (cyl->vars.a == 0.0f)
		return (-1.0f);
	disc = (cyl->vars.b * cyl->vars.b) - (4 * cyl->vars.a * cyl->vars.c);
	if(disc < 0.0f)
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

static float check_best_t(float cyl_part, float best_t, t_cyl *cyl, int index)
{
	if(cyl_part > 0.0f && cyl_part < best_t)
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
		return -1.0f;
	return (best_t);
}

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
