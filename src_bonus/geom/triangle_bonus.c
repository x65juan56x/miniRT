#include "../../include_bonus/minirt_bonus.h"

float	hit_triangle(t_triangle *tr, t_ray r)
{
	float		t;

	tr->vars.e1 = v3_sub(tr->b, tr->a);
	tr->vars.e2 = v3_sub(tr->c, tr->a);
	tr->vars.pvec = v3_cross(r.dir, tr->vars.e2);
	tr->vars.det = v3_dot(tr->vars.e1, tr->vars.pvec);
	if (fabsf(tr->vars.det) < 1e-8f)
		return (-1.0f);
	tr->vars.inv_det = 1.0f / tr->vars.det;
	tr->vars.tvec = v3_sub(r.orig, tr->a);
	tr->vars.u = v3_dot(tr->vars.tvec, tr->vars.pvec) * tr->vars.inv_det;
	if (tr->vars.u < 0.0f || tr->vars.u > 1.0f)
		return (-1.0f);
	tr->vars.qvec = v3_cross(tr->vars.tvec, tr->vars.e1);
	tr->vars.v = v3_dot(r.dir, tr->vars.qvec) * tr->vars.inv_det;
	if (tr->vars.v < 0.0f || (tr->vars.u + tr->vars.v) > 1.0f)
		return (-1.0f);
	t = v3_dot(tr->vars.e2, tr->vars.qvec) * tr->vars.inv_det;
	if (t > 0.0f)
		return (t);
	return (-1.0f);
}
