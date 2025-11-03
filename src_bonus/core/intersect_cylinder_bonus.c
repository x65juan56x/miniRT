#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_internal_bonus.h"

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
/*
* Purpose: Record a ray-cylinder intersection and compute all surface
*	properties for shading.
* Logic: Compute the hit point, delegate to the appropriate surface handler
*	based on which part was hit (wall, top cap, or bottom cap), then
*	finalize the hit record with oriented normal and material properties.
* Math:
*	- p = ray_origin + t * ray_direction: 3D hit point on the cylinder
*	- Normal computation is delegated to specific handlers:
*		* hit_part == 0: curved wall surface (radial normal)
*		* hit_part == 1: top cap (axis-aligned normal)
*		* hit_part == 2: bottom cap (axis-aligned normal)
* Notes: cy->vars.hit_part is set during intersection testing and indicates
*	which part of the cylinder was hit. The function applies surface effects
*	(checkers, bump mapping) via the part-specific handlers, then ensures
*	the normal faces the ray origin and applies specular material properties.
* Use: Called by the intersection dispatcher after hit_cylinder confirms
*	a valid intersection distance t.
* Returns: 1 on success, 0 if hit_part is invalid.
*/
