#include "../../include/minirt.h"
#include "../../include/hit.h"
#include "../../include/shading.h"

int	in_shadow(const t_scene *scene, t_vec3 p, t_vec3 l_pos)
{
	t_vec3	to_l;
	float	max_d;
	t_vec3	dir;
	t_ray	rs;
	t_hit	hit;

	to_l = v3_sub(l_pos, p);
	max_d = v3_len(to_l);
	if (max_d <= EPSILON)
		return (0);
	dir = v3_div(to_l, max_d);
	rs = ray(v3_add(p, v3_mul(dir, EPSILON)), dir);
	if (scene_hit(scene, rs, max_d - EPSILON, &hit))
		return (1);
	return (0);
}
// Cast a ray from p towards light; ignore self with EPSILON and cap max distance
