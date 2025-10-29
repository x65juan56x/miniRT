#include "../../include_bonus/minirt_bonus.h"

// Ispecular = ks * Ilight * max(0, N·H)^shininess
t_vec3
specular_blinn_phong(const t_scene *scene, const t_hit *hit,
	const t_material *material)
{
	t_vec3  zero;
	t_vec3  light_dir;
	t_vec3  view_dir;
	t_vec3  n;
	t_vec3  l;
	t_vec3  v;
	t_vec3  h;
	float   h_len2;
	float   spec_angle;
	float   spec_intensity;
	t_vec3  light_intensity;

	zero = v3(0.0f, 0.0f, 0.0f);
	if (!scene || !hit || !hit->ok || !material)
		return (zero);
	if (material->ks <= 0.0f || material->shininess <= 0.0f)
		return (zero);
	light_dir = v3_sub(scene->light.pos, hit->p);
	view_dir = v3_sub(scene->camera.pos, hit->p);
	if (v3_len2(light_dir) < 1e-10f || v3_len2(view_dir) < 1e-10f)
		return (zero);
	l = v3_norm(light_dir);
	v = v3_norm(view_dir);
	h = v3_add(l, v);
	h_len2 = v3_len2(h);
	if (h_len2 < 1e-10f)
		return (zero);
	h = v3_norm(h);
	n = v3_norm(hit->n);
	spec_angle = v3_dot(n, h);
	if (spec_angle <= 0.0f)
		return (zero);
	spec_intensity = powf(spec_angle, material->shininess);
	if (spec_intensity <= 0.0f)
		return (zero);
	light_intensity = v3_mul(scene->light.color, scene->light.bright);
	return (v3_mul(v3_mul(light_intensity, spec_intensity), material->ks));
}
