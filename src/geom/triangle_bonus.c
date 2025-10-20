#include "../../include/minirt.h"
#include "../../include/hit_bonus.h"

float	hit_triangle(const t_triangle *tr, t_ray r)
{
	t_tr_aux	vars;
	float		t;

	vars.e1 = v3_sub(tr->b, tr->a);
	vars.e2 = v3_sub(tr->c, tr->a);
	vars.pvec = v3_cross(r.dir, vars.e2);
	vars.det = v3_dot(vars.e1, vars.pvec);
	if (fabsf(vars.det) < 1e-8f)
		return (-1.0f);
	vars.inv_det = 1.0f / vars.det;
	vars.tvec = v3_sub(r.orig, tr->a);
	vars.u = v3_dot(vars.tvec, vars.pvec) * vars.inv_det;
	if (vars.u < 0.0f || vars.u > 1.0f)
		return (-1.0f);
	vars.qvec = v3_cross(vars.tvec, vars.e1);
	vars.v = v3_dot(r.dir, vars.qvec) * vars.inv_det;
	if (vars.v < 0.0f || (vars.u + vars.v) > 1.0f)
		return (-1.0f);
	t = v3_dot(vars.e2, vars.qvec) * vars.inv_det;
	if (t > 0.0f)
		return (t);
	return (-1.0f);
}
