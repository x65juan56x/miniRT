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
