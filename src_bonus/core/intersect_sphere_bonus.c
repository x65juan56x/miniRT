#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_bonus_internal.h"

static void	sp_process_checker(t_sphere *sp, t_common_hit *c_hit, t_hit *out)
{
	float		uang;
	t_cb_aux	vars;

	uang = atan2f(c_hit->n.z, c_hit->n.x); // Baldosas aproximadas por arco en unidades de mundo (R * ángulo)
	if (uang < 0.0f)
		uang += 6.283185307179586f; // 2*pi
	vars.iu = (int)floorf((uang * sp->vars.radius) / sp->checker_scale);
	vars.iv = (int)floorf((acosf(c_hit->n.y) * sp->vars.radius)
			/ sp->checker_scale);
	vars.comp = v3_sub(v3(1.0f, 1.0f, 1.0f), sp->color);
	if ((vars.iu + vars.iv) & 1)
		c_hit->albedo = vars.comp;
	else
		c_hit->albedo = sp->color;
	set_common_hit(out, c_hit);
}

static void	sp_process_bump(const t_sphere *sp, t_common_hit *c_hit, t_hit *out)
{
	t_bump_aux	bm_aux;

	// UV from normal
	bm_aux.u = (atan2f(c_hit->n.z, c_hit->n.x)
			+ (float)M_PI) / (2.0f * (float)M_PI);
	bm_aux.v = acosf(c_hit->n.y) / (float)M_PI;
	// Tangent basis from normal
	bm_aux.tangent = v3_cross(v3(0.0f, 1.0f, 0.0f), c_hit->n);
	if (v3_len2(bm_aux.tangent) < 1e-6f)
		bm_aux.tangent = v3_cross(v3(1.0f, 0.0f, 0.0f), c_hit->n);
	bm_aux.tangent = v3_norm(bm_aux.tangent);
	bm_aux.bitangent = v3_cross(c_hit->n, bm_aux.tangent);
	bm_aux.strength = sp->bump_strength;
	bump_perturb(sp->bump, &bm_aux, &out->n);
}

int	record_sphere(t_sphere *sp, t_ray r, float t, t_hit *out)
{
	t_common_hit	c_hit;

	c_hit.p = ray_at(r, t);
	c_hit.n = v3_norm(v3_sub(c_hit.p, sp->center));
	c_hit.t = t;
	c_hit.albedo = sp->color;
	if (sp->has_checker)
		sp_process_checker(sp, &c_hit, out);
	else
		set_common_hit(out, &c_hit);
	if (sp->has_bump && sp->bump)
		sp_process_bump(sp, &c_hit, out);
	orient_normal(out, r);
	apply_specular(out, sp->material);
	return (1);
}
