#include "../../include/minirt.h"
#include "../../include/hit.h"

float	hit_sphere(const t_sphere *sp, t_ray r)
{
	t_sp_aux	vars;
	float		t;

	vars.radius = sp->di * 0.5f;
	vars.oc = v3_sub(r.orig, sp->center);
	vars.a = v3_dot(r.dir, r.dir);
	vars.half_b = v3_dot(vars.oc, r.dir);
	vars.c = v3_dot(vars.oc, vars.oc) - vars.radius * vars.radius;
	vars.disc = vars.half_b * vars.half_b - vars.a * vars.c;
	if (vars.disc < 0.0f)
		return (-1.0f);
	t = (-vars.half_b - sqrtf(vars.disc)) / vars.a;
	if (t > 0.0f)
		return (t);
	t = (-vars.half_b + sqrtf(vars.disc)) / vars.a;
	if (t > 0.0f)
		return (t);
	return (-1.0f);
}
