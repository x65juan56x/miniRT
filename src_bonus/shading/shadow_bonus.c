#include "../../include_bonus/minirt_bonus.h"

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
* Purpose: Check if a surface point is in shadow (blocked from light source).
* Inputs: scene, hit (surface point), l_pos (light position).
* Algorithm:
*   - Calculate direction and distance from hit point to light
*   - If light is extremely close (distance ≈ 0), not in shadow
*   - Cast shadow ray from hit point toward light
*   - Use bias offset to avoid self-intersection (surface blocking itself)
*   - Check if any object blocks the path before reaching the light
* Returns: 1 if in shadow (blocked), 0 if lit (clear path to light).
* Use: Called during lighting calculations to determine if light reaches a point.
*/