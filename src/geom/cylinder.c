#include <float.h>
#include "../../include/minirt.h"
#include "../../include/hit.h"


static int inside_cyl_height(const t_cyl *cylinder, t_vec3 p, t_vec3 v)
{
	float height_pos = v3_dot(v3_sub(p, cylinder->center), v);
	if ((fabsf(height_pos) <= cylinder->he * 0.5f))
		return (1);
	return (0);
}

static float hit_cap(const t_cyl *cylinder, t_ray r, int sign, float radius)
{
	float	t;
	t_vec3	p;
	t_vec3	c_cap;
	t_vec3	radial;

	if (fabsf(v3_dot(r.dir, cylinder->axis)) < 1e-6f)
    	return (-1.0f); //paralelo no hay interseccion
	c_cap = v3_add(cylinder->center, v3_mul(cylinder->axis , sign * (cylinder->he * 0.5f)));
	t = v3_dot(v3_sub(c_cap, r.orig), cylinder->axis) / v3_dot(r.dir, cylinder->axis);
	if(t < 0.0f)
		return (-1.0f);
	p = v3_add(r.orig, v3_mul(r.dir, t));
	radial = v3_sub(p, c_cap);
	if(v3_dot(radial, radial) <= radius * radius)
		return (t);
	return (-1.0f);
}

typedef struct s_quad {
    float a;
    float b;
    float c;
}	t_quad;

static t_quad cyl_quadratic(const t_cyl *cylinder, t_ray r, t_vec3 x, t_vec3 v)
{
	t_quad q;

	ft_memset(&q, 0,sizeof(t_quad));
	float x_dot_v = v3_dot(x, v);
	float d_dot_v = v3_dot(r.dir, v);

	q.a = v3_dot(v3_sub(r.dir, v3_mul(v, d_dot_v)), v3_sub(r.dir, v3_mul(v, d_dot_v)));
	q.b = 2.0f * v3_dot(v3_sub(r.dir, v3_mul(v, d_dot_v)), v3_sub(x, v3_mul(v, x_dot_v)));
	q.c = (v3_dot(v3_sub(x, v3_mul(v, x_dot_v)), v3_sub(x, v3_mul(v, x_dot_v)))) - (cylinder->di*0.5f) * (cylinder->di*0.5f);
	return q;
}

static float pick_valid_t(const t_cyl *cylinder, t_ray r, float t1, float t2)
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
	if(tmin > 0.0f)
	{
		p = v3_add(r.orig, v3_mul(r.dir, tmin));
		if (inside_cyl_height(cylinder, p, cylinder->axis))
			return (tmin);
	}
	if(tmax > 0.0f && tmax >= tmin)
	{
		p = v3_add(r.orig, v3_mul(r.dir, tmax));
		if (inside_cyl_height(cylinder, p, cylinder->axis))
			return (tmax);
	}
	return (-1.0f);
}

static float hit_side(const t_cyl *cyl, t_ray r)
{
	t_quad	q;
	float	disc;
	float	t1;
	float	t2;
	float	tmp;

	q = cyl_quadratic(cyl, r,v3_sub(r.orig, cyl->center), cyl->axis);
	if (q.a == 0.0f)
		return (-1.0f);
	disc = (q.b * q.b) - (4 * q.a * q.c);
	if(disc < 0.0f)
		return (-1.0f);
	t1 = (-q.b - sqrt(disc)) / (2.0f*q.a);
	t2 = (-q.b + sqrt(disc)) / (2.0f*q.a);
	if (t1 > t2)
	{
		tmp = t1;
		t1 = t2;
		t2 = tmp;
	}
	return (pick_valid_t(cyl, r, t1, t2));
}

float	hit_cylinder(const t_cyl *cylinder, t_ray r, int *hit_part)
{
	float	best_t;
	float	t_side;
	float	radius;
	float	t_top;
	float	t_bottom;

	best_t = FLT_MAX;
	t_side = hit_side(cylinder, r);
	radius = cylinder->di * 0.5f;
	*hit_part = -1;
	if(t_side > 0.0f && t_side < best_t)
	{
		best_t = t_side;
		*hit_part = 0;
	}
	t_top = hit_cap(cylinder, r, 1, radius);
	if(t_top > 0.0f && t_top < best_t)
	{
		best_t = t_top;
		*hit_part = 1;
	}
	t_bottom = hit_cap(cylinder, r, -1, radius);
	if(t_bottom > 0.0f && t_bottom < best_t)
	{
		best_t = t_bottom;
		*hit_part = 2;
	}
	if (best_t >= FLT_MAX || *hit_part == -1)
        return -1.0f;
	return best_t;
}
