#include "../../include/minirt.h"
#include "../../include/hit.h"


static int inside_cyl_height(const t_cyl *cylinder, t_vec3 p, t_vec3 v)
{
	float height_pos = v3_dot(v3_sub(p, cylinder->center), v);
	if ((fabsf(height_pos) <= cylinder->he * 0.5f))
		return 1;
	return 0;
}

static float hit_top_cap(const t_cyl *cylinder, t_ray r, t_vec3 v)
{
	float t;
	//float denom = v3_dot(r.dir, v);
	t_vec3 p;
	t_vec3 c_top;
	float radio;
	t_vec3 radial;

	if (fabsf(v3_dot(r.dir, v)) < 1e-6f)
    	return (-1.0f); //paralelo no hay interseccion
	c_top = v3_add(cylinder->center, v3_mul(v ,(cylinder->he * 0.5f)));
	t = v3_dot(v3_sub(c_top, r.orig), v) / v3_dot(r.dir, v);
	p = v3_add(r.orig, v3_mul(r.dir, t));
	radio = cylinder->di * 0.5f;
	radial = v3_sub(p, c_top);
	if(t < 0.0f)
		return (-1.0f);
	if(v3_dot(radial, radial) <= radio * radio)
		return t;
	return (-1.0f);
}

static float hit_botton_cap(const t_cyl *cylinder, t_ray r, t_vec3 v)
{
	float t;
	//float denom = v3_dot(r.dir, v);
	t_vec3 p;
	t_vec3 c_bottom;
	float radio;
	t_vec3 radial;
	
	if (fabsf(v3_dot(r.dir, v)) < 1e-6f)
    	return (-1.0f); //paralelo no hay interseccion
	c_bottom = v3_sub(cylinder->center, v3_mul(v ,(cylinder->he * 0.5f)));
	t = v3_dot(v3_sub(c_bottom, r.orig), v) / v3_dot(r.dir, v);
	p = v3_add(r.orig, v3_mul(r.dir, t));
	radio = cylinder->di * 0.5f;
	radial = v3_sub(p, c_bottom);
	if(t < 0.0f)
		return (-1.0f);
	if(v3_dot(radial, radial) <= radio * radio)
		return t;
	return (-1.0f);
}

typedef struct s_quad {
    float a;
    float b;
    float c;
} t_quad;

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
	float t = 1e30;
	t_vec3 v = v3_norm(cylinder->axis);

	if(t1 > 0.0f)
	{
		t_vec3 p1 = v3_add(r.orig, v3_mul(r.dir, t1));
		if (inside_cyl_height(cylinder, p1, v))
			t = t1;
	}
	if(t2 > 0.0f)
	{
		t_vec3 p2 = v3_add(r.orig, v3_mul(r.dir, t2));
		if (inside_cyl_height(cylinder, p2, v) && t2 < t)
			t = t2;
	}
	if(t < 1e29f)
		return(t);
	return (-1.0f);
}

static float hit_side(const t_cyl *cylinder, t_ray r, t_vec3 v)
{
	t_vec3	x;
	t_quad	q;
	float disc;
	float t1;
	float t2;

	x = v3_sub(r.orig, cylinder->center);
	q = cyl_quadratic(cylinder, r, x, v);
	if (q.a == 0.0f)
		return -1.0f;
	disc = (q.b * q.b) - (4 * q.a * q.c);
	if(disc < 0.0f)
		return (-1.0f);
	t1 = (-q.b - sqrt(disc)) / (2.0f*q.a);
	t2 = (-q.b + sqrt(disc)) / (2.0f*q.a);
	return(pick_valid_t(cylinder, r, t1, t2));
}

float	hit_cylinder(const t_cyl *cylinder, t_ray r, int *hit_part)
{
	t_vec3 v = v3_norm(cylinder->axis);
	float best_t = 1e30f;
	float t_side = hit_side(cylinder, r, v);

	*hit_part = -1;
	if(t_side > 0.0f && t_side < best_t)
	{
		best_t = t_side;
		*hit_part = 0;
	}
	float t_top = hit_top_cap(cylinder, r, v);
	if(t_top > 0.0f && t_top < best_t)
	{
		best_t = t_top;
		*hit_part = 1;
	}
	float t_bottom = hit_botton_cap(cylinder, r, v);
	if(t_bottom > 0.0f && t_bottom < best_t)
	{
		best_t = t_bottom;
		*hit_part = 2;
	}
	if (best_t >= 1e30f || *hit_part == -1)
        return -1.0f;
	return best_t;
}
