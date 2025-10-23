#include "../../include/minirt.h"
#include "../../include/scene.h"
#include "../../include/hit.h"

static void	set_common_hit(t_hit *dst, float t, t_vec3 p, t_vec3 n, t_vec3 albedo)
{
	dst->ok = 1;
	dst->t = t;
	dst->p = p;
	dst->n = n;
	dst->albedo = albedo;
}

static void	orient_normal(t_hit *hit, t_ray r)
{
	if (v3_dot(hit->n, r.dir) > 0.0f)
		hit->n = v3_mul(hit->n, -1.0f);
}

static int	record_sphere(const t_sphere *sp, t_ray r, float t, t_hit *out)
{
	t_vec3	p;
	t_vec3	n;

	p = ray_at(r, t);
	n = v3_norm(v3_sub(p, sp->center));
	set_common_hit(out, t, p, n, sp->color);
	orient_normal(out, r);
	return (1);
}

static int	record_plane(const t_plane *pl, t_ray r, float t, t_hit *out)
{
	t_vec3	p;

	p = ray_at(r, t);
	set_common_hit(out, t, p, pl->normal, pl->color);
	orient_normal(out, r);
	return (1);
}

t_vec3 normal_cyl(const t_cyl *cylinder, t_vec3 p)
{
	t_vec3 radial;
	radial = v3_sub(p, cylinder->center);
	float axial = v3_dot(radial, cylinder->axis);
	//vector perpendicular
	radial = v3_sub(radial, v3_mul(cylinder->axis, axial));
	return (radial);
}

static int record_cylinder(const t_cyl *cy, t_ray r, float t, t_hit *out, int hit_part)
{
	t_vec3 p;
	t_vec3 n;
	p = ray_at(r, t);
    if (hit_part == 0)
        n = v3_norm(normal_cyl(cy, p));
    else if (hit_part == 1)
        n = v3_norm(cy->axis);
    else if (hit_part == 2)
        n = v3_norm(v3_mul(cy->axis, -1));
    else
    {
		return 0;
	}
	set_common_hit(out, t, p, n, cy->color);
	orient_normal(out, r);
	return (1);
}

static int	object_hit(const t_object *obj, t_ray r, t_hit *out)
{
	float	t;
	int		hit_part;

	t = -1.0f;
	hit_part = -1.0f;
	if (obj->type == OBJ_SPHERE)
		t = hit_sphere(&obj->u_obj.sp, r);
	else if (obj->type == OBJ_PLANE)
		t = hit_plane(&obj->u_obj.pl, r);
	else if (obj->type == OBJ_CYLINDER)
		t = hit_cylinder(&obj->u_obj.cy, r, &hit_part);
	if (t <= 0.0f)
		return (0);
	if (obj->type == OBJ_SPHERE)
		return (record_sphere(&obj->u_obj.sp, r, t, out));
	if (obj->type == OBJ_PLANE)
		return (record_plane(&obj->u_obj.pl, r, t, out));
	return (record_cylinder(&obj->u_obj.cy, r, t, out, hit_part)); // cambiar por record_cylinder cuando esté lista
}

int	scene_hit(const t_scene *scene, t_ray r, float max_dist, t_hit *out)
{
	const t_object	*o;
	float			best;
	t_hit			cur;
	int				found;

	out->ok = 0;
	found = 0;
	best = max_dist;
	o = scene->objects;
	while (o)
	{
		if (object_hit(o, r, &cur) && cur.t > EPSILON && cur.t < best)
		{
			best = cur.t;
			*out = cur;
			found = 1;
		}
		o = o->next;
	}
	return (found);
}
