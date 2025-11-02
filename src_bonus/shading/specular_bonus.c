#include "../../include_bonus/minirt_bonus.h"

// Ispecular = ks * Ilight * max(0, N·H)^shininess
t_vec3	specular_blinn_phong(const t_scene *scene, const t_light *light,
	const t_hit *hit, t_vec3 light_dir)
{
	t_spec_vars	vars;
	t_vec3		half_vec;
	float		h_len2;

	if (!scene || !light || !hit || !hit->ok)
		return (v3(0.0f, 0.0f, 0.0f));
	if (hit->ks <= 0.0f || hit->shininess <= 0.0f || light->bright <= 0.0f)
		return (v3(0.0f, 0.0f, 0.0f));
	vars.view_dir = v3_sub(scene->camera.pos, hit->p);
	if (v3_len2(vars.view_dir) < 1e-10f)
		return (v3(0.0f, 0.0f, 0.0f));
	vars.view_dir = v3_norm(vars.view_dir);
	half_vec = v3_add(light_dir, vars.view_dir);
	h_len2 = v3_len2(half_vec);
	if (h_len2 < 1e-10f)
		return (v3(0.0f, 0.0f, 0.0f));
	half_vec = v3_mul(half_vec, 1.0f / sqrtf(h_len2));
	vars.spec_angle = v3_dot(hit->n, half_vec);
	if (vars.spec_angle <= 0.0f)
		return (v3(0.0f, 0.0f, 0.0f));
	vars.spec_intensity = powf(vars.spec_angle, hit->shininess);
	if (vars.spec_intensity <= 0.0f)
		return (v3(0.0f, 0.0f, 0.0f));
	vars.light_intensity = v3_mul(light->color, light->bright);
	return (v3_mul(vars.light_intensity, hit->ks * vars.spec_intensity));
}

t_vec3	specular_phong(const t_scene *scene, const t_light *light,
	const t_hit *hit, t_vec3 l_dir)
{
	t_spec_vars	vars;
	t_vec3		reflect_dir;
	float		r_len2;

	if (!scene || !light || !hit || !hit->ok)
		return (v3(0.0f, 0.0f, 0.0f));
	if (hit->ks <= 0.0f || hit->shininess <= 0.0f || light->bright <= 0.0f)
		return (v3(0.0f, 0.0f, 0.0f));
	vars.view_dir = v3_sub(scene->camera.pos, hit->p);
	if (v3_len2(vars.view_dir) < 1e-10f)
		return (v3(0.0f, 0.0f, 0.0f));
	vars.view_dir = v3_norm(vars.view_dir);
	reflect_dir = v3_sub(v3_mul(hit->n, 2.0f * v3_dot(hit->n, l_dir)), l_dir);
	r_len2 = v3_len2(reflect_dir);
	if (r_len2 < 1e-10f)
		return (v3(0.0f, 0.0f, 0.0f));
	reflect_dir = v3_mul(reflect_dir, 1.0f / sqrtf(r_len2));
	vars.spec_angle = v3_dot(reflect_dir, vars.view_dir);
	if (vars.spec_angle <= 0.0f)
		return (v3(0.0f, 0.0f, 0.0f));
	vars.spec_intensity = powf(vars.spec_angle, hit->shininess);
	if (vars.spec_intensity <= 0.0f)
		return (v3(0.0f, 0.0f, 0.0f));
	vars.light_intensity = v3_mul(light->color, light->bright);
	return (v3_mul(vars.light_intensity, hit->ks * vars.spec_intensity));
}
