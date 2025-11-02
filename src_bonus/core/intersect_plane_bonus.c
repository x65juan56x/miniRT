#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_bonus_internal.h"

void	pl_process_checker(t_plane *pl, t_common_hit *c_hit, t_hit *out)
{
	t_vec3		rel;
	t_cb_aux	vars;

	rel = v3_sub(c_hit->p, pl->point);
	vars.iu = (int)floorf(v3_dot(rel, pl->vars.u) / pl->checker_scale);
	vars.iv = (int)floorf(v3_dot(rel, pl->vars.v) / pl->checker_scale);
	vars.comp = v3_sub(v3(1.0f, 1.0f, 1.0f), pl->color);
	if ((vars.iu + vars.iv) & 1)
		c_hit->albedo = vars.comp; // color complementario
	set_common_hit(out, c_hit);
}

void	pl_process_bump(const t_plane *pl, t_common_hit *c_hit, t_hit *out)
{
	t_bump_aux	bm_aux;
	t_vec3		rel;

	rel = v3_sub(c_hit->p, pl->point);
	if (pl->checker_scale > 0.0f)
		bm_aux.u = v3_dot(rel, pl->vars.u) / pl->checker_scale;
	else
		bm_aux.u = v3_dot(rel, pl->vars.u);
	if (pl->checker_scale > 0.0f)
		bm_aux.v = v3_dot(rel, pl->vars.v) / pl->checker_scale;
	else
		bm_aux.v = v3_dot(rel, pl->vars.v);
	bm_aux.tangent = pl->vars.u;
	bm_aux.bitangent = pl->vars.v;
	bm_aux.strength = pl->bump_strength;
	bump_perturb(pl->bump, &bm_aux, &out->n);
}

int	record_plane(t_plane *pl, t_ray r, float t, t_hit *out)
{
	t_common_hit	c_hit;

	c_hit.p = ray_at(r, t);
	c_hit.t = t;
	c_hit.n = pl->normal;
	c_hit.albedo = pl->color;
	if (pl->has_checker)
		pl_process_checker(pl, &c_hit, out);
	else
		set_common_hit(out, &c_hit);
	if (pl->has_bump && pl->bump)
		pl_process_bump(pl, &c_hit, out);
	orient_normal(out, r);
	apply_specular(out, pl->material);
	return (1);
}
