#include "../../include/minirt.h"
#include "../../include/shading.h"
#include "../../include/scene.h"

t_vec3	shade_lambert(const t_scene *scene, const t_hit *hit)
{
	t_vec3	ambient;
	t_vec3	l_dir;
	float	ndotl;
	t_vec3	diff;
	t_vec3	c;

	if (!hit->ok)
		return (v3(0, 0, 0));
	ambient = v3_mul(scene->ambient.color, scene->ambient.ratio);
	if (in_shadow(scene, hit, scene->light.pos))
		return (v3_ctoc(hit->albedo, ambient));
	l_dir = v3_norm(v3_sub(scene->light.pos, hit->p));
	ndotl = v3_dot(hit->n, l_dir);
	if (ndotl < 0.0f)
		ndotl = 0.0f;
	diff = v3_mul(v3_mul(scene->light.color, scene->light.bright), ndotl);
	c = v3_add(ambient, v3_ctoc(hit->albedo, diff));
	return (c);
}
/*
* Purpose: Compute surface color using Lambert (diffuse) shading.
* Logic: ambient + (albedo × diffuse), where diffuse = light × max(0, N·L).
* Notes: Returns only ambient if in shadow or no hit (black if miss).
*/
