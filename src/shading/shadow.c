#include "../../include/minirt.h"
#include "../../include/shading.h"
#include "../../include/scene.h"

int	in_shadow(const t_scene *scene, const t_hit *hit, t_vec3 l_pos)
{
	t_vec3	to_l;
	float	max_d;
	t_vec3	dir;
	t_hit	block;
	float	bias;

	to_l = v3_sub(l_pos, hit->p);
	max_d = v3_len(to_l);
	if (max_d <= EPSILON)
		return (0);
	dir = v3_div(to_l, max_d);
	bias = fmaxf(1e-4f, 1e-3f * hit->t);
	if (scene_hit(scene, ray(v3_add(hit->p, v3_mul(dir, bias)), dir),
			max_d - bias, &block))
		return (1);
	return (0);
}
/*
* Purpose: Test if a point is in shadow (blocked from light source).
* Logic: Cast ray from hit point toward light; if any object intersects,
	it's in shadow.
* Notes: bias offsets ray origin to avoid self-intersection; max_d ensures
	occlusion only within light distance.
*/
