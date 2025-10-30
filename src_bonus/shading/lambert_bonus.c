#include "../../include_bonus/minirt_bonus.h"

static t_vec3	compute_specular(const t_scene *scene, const t_light *light,
	const t_hit *hit, t_vec3 light_dir)
{
	if (hit->spec_model == SPEC_MODEL_PHONG)
		return (specular_phong(scene, light, hit, light_dir));
	return (specular_blinn_phong(scene, light, hit, light_dir));
}

t_vec3	shade_lambert(const t_scene *scene, const t_hit *hit)
{
	t_vec3	ambient_term;
	t_vec3	diffuse_total;
	t_vec3	specular_total;
//	t_vec3	normal;
	t_light	*light;

	if (!hit->ok)
		return (v3(0.0f, 0.0f, 0.0f));
	ambient_term = v3_ctoc(hit->albedo,
		v3_mul(scene->ambient.color, scene->ambient.ratio));
	diffuse_total = v3(0.0f, 0.0f, 0.0f);
	specular_total = v3(0.0f, 0.0f, 0.0f);
//	normal = v3_norm(hit->n);
	light = scene->light;
	while (light)
	{
		t_vec3	to_light;
		t_vec3	light_dir;
		float	ndotl;

		if (light->bright <= 0.0f)
		{
			light = light->next;
			continue;
		}
		to_light = v3_sub(light->pos, hit->p);
		if (v3_len2(to_light) < 1e-10f)
		{
			light = light->next;
			continue;
		}
		light_dir = v3_norm(to_light);
		ndotl = v3_dot(hit->n, light_dir);
		if (ndotl <= 0.0f)
		{
			light = light->next;
			continue;
		}
		if (in_shadow(scene, hit, light->pos))
		{
			light = light->next;
			continue;
		}
		diffuse_total = v3_add(diffuse_total,
			v3_mul(v3_mul(light->color, light->bright), ndotl));
		if (hit->ks > 0.0f && hit->shininess > 0.0f)
			specular_total = v3_add(specular_total,
				compute_specular(scene, light, hit, light_dir));
		light = light->next;
	}
	return (v3_add(v3_add(ambient_term, v3_ctoc(hit->albedo, diffuse_total)),
		specular_total));
}
