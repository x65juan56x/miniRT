#include "../../include/minirt_bonus.h"

int	in_shadow(const t_scene *scene, const t_hit *hit, t_vec3 l_pos)
{
	t_vec3	to_l;
	float	max_d;
	t_vec3	dir;
	t_ray	rs;
	t_hit	block;
	float	bias;

	to_l = v3_sub(l_pos, hit->p);
	max_d = v3_len(to_l);
	if (max_d <= EPSILON)
		return (0);
	dir = v3_div(to_l, max_d);
	bias = fmaxf(1e-4f, 1e-3f * hit->t);
	rs = ray(v3_add(hit->p, v3_mul(dir, bias)), dir);
	if (scene_hit(scene, rs, max_d - bias, &block))
		return (1);
	return (0);
}
// Cast a ray from p towards light; ignore self with EPSILON and cap max distance
