#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_bonus_internal.h"

int	record_cylinder(t_cyl *cy, t_ray r, float t, t_hit *out)
{
	t_common_hit	c_hit;

	c_hit.p = ray_at(r, t);
	c_hit.t = t;
	c_hit.albedo = cy->color;
	if (cy->vars.hit_part == 0)
		cy_hit_wall(cy, &c_hit, out);
	else if (cy->vars.hit_part == 1)
		cy_hit_top(cy, &c_hit, out);
	else if (cy->vars.hit_part == 2)
		cy_hit_bottom(cy, &c_hit, out);
	else
		return (0);
	orient_normal(out, r);
	apply_specular(out, cy->material);
	return (1);
}
