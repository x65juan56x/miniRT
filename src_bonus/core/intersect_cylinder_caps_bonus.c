#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_bonus_internal.h"

static void	cycap_process_checker(t_cyl *cy, t_common_hit *c_hit, t_hit *out,
		t_vec3 q)
{
	t_cb_aux	vars;

	vars.iu = (int)floorf(v3_dot(q, cy->vars.base_u) / cy->checker_scale);
	vars.iv = (int)floorf(v3_dot(q, cy->vars.base_v) / cy->checker_scale);
	vars.comp = v3_sub(v3(1.0f, 1.0f, 1.0f), cy->color);
	if ((vars.iu + vars.iv) & 1)
		c_hit->albedo = vars.comp;
	set_common_hit(out, c_hit);
}

static void	cycap_process_bump(t_cyl *cy, t_hit *out, t_vec3 q)
{
	t_bump_aux	bm_aux;
	float		x;
	float		y;

	x = v3_dot(q, cy->vars.base_u);
	y = v3_dot(q, cy->vars.base_v);
	// Normalize to [0,1] across the disk extent
	bm_aux.u = (x / cy->vars.radius) * 0.5f + 0.5f;
	bm_aux.v = (y / cy->vars.radius) * 0.5f + 0.5f;
	bm_aux.tangent = cy->vars.base_u;
	bm_aux.bitangent = cy->vars.base_v;
	bm_aux.strength = cy->bump_strength;
	bump_perturb(cy->bump, &bm_aux, &out->n);
}

void	cy_hit_top(t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	t_vec3		ctop;
	t_vec3		q;

	c_hit->n = cy->axis;
	ctop = v3_add(cy->center, v3_mul(cy->axis, cy->vars.half_height));
	q = v3_sub(c_hit->p, ctop);
	if (cy->has_checker)
		cycap_process_checker(cy, c_hit, out, q);
	else
		set_common_hit(out, c_hit);
	if (cy->has_bump && cy->bump)
		cycap_process_bump(cy, out, q);
}

void	cy_hit_bottom(t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	t_vec3		cbot;
	t_vec3		q;

	c_hit->n = v3_mul(cy->axis, -1.0f);
	cbot = v3_sub(cy->center, v3_mul(cy->axis, cy->vars.half_height));
	q = v3_sub(c_hit->p, cbot);
	if (cy->has_checker)
		cycap_process_checker(cy, c_hit, out, q);
	else
		set_common_hit(out, c_hit);
	if (cy->has_bump && cy->bump)
		cycap_process_bump(cy, out, q);
}
