#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_internal_bonus.h"

static t_vec3	hp_checker_color(const t_hparab *hp, float x, float y)
{
	t_cb_aux	vars;

	vars.iu = (int)floorf(x / hp->checker_scale);
	vars.iv = (int)floorf(y / hp->checker_scale);
	vars.comp = v3_sub(v3(1.0f, 1.0f, 1.0f), hp->color);
	if ((vars.iu + vars.iv) & 1)
		return (vars.comp);
	return (hp->color);
}

static void	hp_process_bump(const t_hparab *hp, float x, float y, t_hit *out)
{
	t_bump_aux	bm_aux;

	bm_aux.u = (x / hp->rx) * 0.5f + 0.5f;
	bm_aux.v = (y / hp->ry) * 0.5f + 0.5f;
	bm_aux.tangent = hp->vars.u;
	bm_aux.bitangent = hp->vars.v;
	bm_aux.strength = hp->bump_strength;
	bump_perturb(hp->bump, &bm_aux, &out->n);
}

int	record_hparaboloid(const t_hparab *hp, t_ray r, float t, t_hit *out)
{
	t_common_hit	c_hit;
	float			x;
	float			y;

	c_hit.p = ray_at(r, t);
	c_hit.t = t;
	x = v3_dot(v3_sub(c_hit.p, hp->center), hp->vars.u);
	y = v3_dot(v3_sub(c_hit.p, hp->center), hp->vars.v);
	c_hit.n = v3_add(v3_add(v3_mul(hp->vars.u, -2.0f * x * hp->vars.inv_rx2),
				v3_mul(hp->vars.v, 2.0f * y * hp->vars.inv_ry2)),
			v3_mul(hp->axis, -hp->vars.inv_height));
	c_hit.n = v3_norm(c_hit.n);
	c_hit.albedo = hp->color;
	if (hp->has_checker)
		c_hit.albedo = hp_checker_color(hp, x, y);
	set_common_hit(out, &c_hit);
	if (hp->has_bump && hp->bump)
		hp_process_bump(hp, x, y, out);
	orient_normal(out, r);
	apply_specular(out, hp->material);
	return (1);
}
