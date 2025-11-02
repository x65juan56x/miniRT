#include "../../include_bonus/minirt_bonus.h"

void	set_common_hit(t_hit *dst, t_common_hit *c_hit)
{
	dst->ok = 1;
	dst->t = c_hit->t;
	dst->p = c_hit->p;
	dst->n = c_hit->n;
	dst->albedo = c_hit->albedo;
	dst->ks = 0.0f;
	dst->shininess = 0.0f;
	dst->spec_model = SPEC_MODEL_BLINN;
	dst->reflectivity = 0.0f;
	dst->specular = v3(0.0f, 0.0f, 0.0f);
}

void	orient_normal(t_hit *hit, t_ray r)
{
	if (v3_dot(hit->n, r.dir) > 0.0f)
		hit->n = v3_mul(hit->n, -1.0f);
}

void	apply_specular(t_hit *out, const t_material *material)
{
	out->specular = v3(0.0f, 0.0f, 0.0f);
	out->ks = 0.0f;
	out->shininess = 0.0f;
	out->spec_model = SPEC_MODEL_BLINN;
	out->reflectivity = 0.0f;
	if (!material)
		return ;
	out->ks = material->ks;
	out->shininess = material->shininess;
	out->spec_model = material->model;
	out->reflectivity = material->reflectivity;
	if (material->ks <= 0.0f || material->shininess <= 0.0f)
	{
		out->ks = 0.0f;
		out->shininess = 0.0f;
		out->spec_model = SPEC_MODEL_BLINN;
	}
}
