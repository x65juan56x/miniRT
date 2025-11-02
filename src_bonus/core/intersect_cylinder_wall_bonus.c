#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_bonus_internal.h"

static t_vec3	normal_cyl(const t_cyl *cylinder, t_vec3 p)
{
	t_vec3	radial;
	float	axial;

	radial = v3_sub(p, cylinder->center);
	axial = v3_dot(radial, cylinder->axis);
	radial = v3_sub(radial, v3_mul(cylinder->axis, axial));
	return (radial);
}

static void	cywall_process_checker(t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	t_cb_aux	vars;
	t_vec3		rel;
	float		h;
	t_vec3		radial;
	float		theta;

	rel = v3_sub(c_hit->p, cy->center);
	h = v3_dot(rel, cy->axis);
	radial = v3_norm(v3_sub(rel, v3_mul(cy->axis, h)));
	theta = atan2f(v3_dot(radial, cy->vars.base_v),
			v3_dot(radial, cy->vars.base_u));
	if (theta < 0.0f)
		theta += 6.283185307179586f; // 2*pi
	vars.iu = (int)floorf((cy->vars.radius * theta) / cy->checker_scale);
	vars.iv = (int)floorf(((h + cy->vars.half_height)) / cy->checker_scale);
	vars.comp = v3_sub(v3(1.0f, 1.0f, 1.0f), cy->color);
	if ((vars.iu + vars.iv) & 1)
		c_hit->albedo = vars.comp;
	set_common_hit(out, c_hit);
}

static void	cywall_process_bump(t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	t_bump_aux	bm_aux;
	t_vec3		rel;
	float		h;
	t_vec3		radial;
	float		theta;

	rel = v3_sub(c_hit->p, cy->center);
	h = v3_dot(rel, cy->axis);
	radial = v3_norm(v3_sub(rel, v3_mul(cy->axis, h)));
	theta = atan2f(v3_dot(radial, cy->vars.base_v),
			v3_dot(radial, cy->vars.base_u));
	if (theta < 0.0f)
		theta += 6.283185307179586f; // 2*pi
	bm_aux.u = theta / (2.0f * (float)M_PI);
	bm_aux.v = (h + cy->vars.half_height) / cy->he;
	// Tangent basis: around azimuth and along axis
	bm_aux.tangent = v3_norm(v3_cross(cy->axis, c_hit->n));
	if (v3_len2(bm_aux.tangent) < 1e-8f)
		bm_aux.tangent = cy->vars.base_u; // fallback
	bm_aux.bitangent = cy->axis;
	bm_aux.strength = cy->bump_strength;
	bump_perturb(cy->bump, &bm_aux, &out->n);
}

void	cy_hit_wall(t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	c_hit->n = v3_norm(normal_cyl(cy, c_hit->p));
	if (cy->has_checker)
		cywall_process_checker(cy, c_hit, out);
	else
		set_common_hit(out, c_hit);
	// Bump on side: cylindrical UV in [0,1]x[0,1]
	if (cy->has_bump && cy->bump)
		cywall_process_bump(cy, c_hit, out);
}
