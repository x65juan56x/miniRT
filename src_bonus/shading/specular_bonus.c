#include "../../include_bonus/minirt_bonus.h"

// Ispecular = ks * Ilight * max(0, N·H)^shininess
t_vec3
specular_blinn_phong(const t_scene *scene, const t_light *light,
	const t_hit *hit, t_vec3 light_dir)
{
	t_vec3	zero;
	t_vec3	view_dir;
	t_vec3	half_vec;
	float	h_len2;
	float	spec_angle;
	float	spec_intensity;
	t_vec3	light_intensity;

	zero = v3(0.0f, 0.0f, 0.0f);
	if (!scene || !light || !hit || !hit->ok)
		return (zero);
	if (hit->ks <= 0.0f || hit->shininess <= 0.0f || light->bright <= 0.0f)
		return (zero);
	view_dir = v3_sub(scene->camera.pos, hit->p);
	if (v3_len2(view_dir) < 1e-10f)
		return (zero);
	view_dir = v3_norm(view_dir);
	half_vec = v3_add(light_dir, view_dir);
	h_len2 = v3_len2(half_vec);
	if (h_len2 < 1e-10f)
		return (zero);
	half_vec = v3_mul(half_vec, 1.0f / sqrtf(h_len2));
	spec_angle = v3_dot(hit->n, half_vec);
	if (spec_angle <= 0.0f)
		return (zero);
	spec_intensity = powf(spec_angle, hit->shininess);
	if (spec_intensity <= 0.0f)
		return (zero);
	light_intensity = v3_mul(light->color, light->bright);
	return (v3_mul(light_intensity, hit->ks * spec_intensity));
}
