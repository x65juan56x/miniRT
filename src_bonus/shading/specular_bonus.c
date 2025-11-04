#include "../../include_bonus/minirt_bonus.h"

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
/*
* Purpose: Calculate specular highlight using Blinn-Phong model.
* Inputs: scene, light, hit (surface point with material), light_dir.
* Algorithm:
*   - Validate inputs and material properties (ks, shininess)
*   - Calculate view direction (from surface to camera)
*   - Compute halfway vector H: average of light direction and view direction
*   - Calculate N·H: angle between surface normal and halfway vector
*   - Raise to shininess power: (N·H)^shininess
*       • Higher shininess = sharper, smaller highlight
*       • Lower shininess = broader, softer highlight
*   - Scale by ks (specular strength) and light intensity
* Formula: specular = ks * light * (N·H)^shininess
* Notes: Blinn-Phong is faster than Phong (uses halfway vector instead of reflection).
* Returns: Specular contribution RGB color.
* Use: Called when computing shiny highlights on reflective surfaces.
*/

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
/*
* Purpose: Calculate specular highlight using classic Phong model.
* Inputs: scene, light, hit (surface point), l_dir (light direction).
* Algorithm:
*   - Validate inputs and material properties
*   - Calculate view direction (from surface to camera)
*   - Compute reflection direction R: mirror reflection of light across normal
*       • R = 2(N·L)N - L
*   - Calculate R·V: angle between reflection and view direction
*   - Raise to shininess power: (R·V)^shininess
*   - Scale by ks and light intensity
* Formula: specular = ks * light * (R·V)^shininess
* Notes: Original Phong model. More expensive than Blinn-Phong but slightly different look.
*        Best visible when view direction aligns with perfect reflection.
* Returns: Specular contribution RGB color.
* Use: Alternative specular model when SPEC_MODEL_PHONG is selected.
*/
