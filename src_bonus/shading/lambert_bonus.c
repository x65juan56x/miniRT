#include "../../include_bonus/minirt_bonus.h"

static t_vec3	compute_specular(const t_scene *scene, const t_light *light,
	const t_hit *hit, t_vec3 light_dir)
{
	if (hit->spec_model == SPEC_MODEL_PHONG)
		return (specular_phong(scene, light, hit, light_dir));
	return (specular_blinn_phong(scene, light, hit, light_dir));
}

static int	init_light_sample(const t_scene *scene, const t_hit *hit,
	const t_light *light, t_light_sample *sample)
{
	t_vec3	to_light;

	if (light->bright <= 0.0f)
		return (0);
	to_light = v3_sub(light->pos, hit->p);
	if (v3_len2(to_light) < 1e-10f)
		return (0);
	sample->dir = v3_norm(to_light);
	sample->ndotl = v3_dot(hit->n, sample->dir);
	if (sample->ndotl <= 0.0f)
		return (0);
	if (in_shadow(scene, hit, light->pos))
		return (0);
	return (1);
}

static void	accumulate_light(const t_scene *scene, const t_hit *hit,
	t_vec3 *diffuse_total, t_vec3 *specular_total)
{
	t_light			*light;
	t_light_sample	sample;

	light = scene->light;
	while (light)
	{
		if (init_light_sample(scene, hit, light, &sample))
		{
			*diffuse_total = v3_add(*diffuse_total,
					v3_mul(v3_mul(light->color, light->bright),
						sample.ndotl));
			if (hit->ks > 0.0f && hit->shininess > 0.0f)
				*specular_total = v3_add(*specular_total,
						compute_specular(scene, light, hit,
							sample.dir));
		}
		light = light->next;
	}
}

t_vec3	shade_lambert(const t_scene *scene, const t_hit *hit)
{
	t_vec3	ambient_term;
	t_vec3	diffuse_total;
	t_vec3	specular_total;

	if (!hit->ok)
		return (v3(0.0f, 0.0f, 0.0f));
	ambient_term = v3_ctoc(hit->albedo,
			v3_mul(scene->ambient.color, scene->ambient.ratio));
	diffuse_total = v3(0.0f, 0.0f, 0.0f);
	specular_total = v3(0.0f, 0.0f, 0.0f);
	accumulate_light(scene, hit, &diffuse_total, &specular_total);
	return (v3_add(v3_add(ambient_term, v3_ctoc(hit->albedo, diffuse_total)),
			specular_total));
}