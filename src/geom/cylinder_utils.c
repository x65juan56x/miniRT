#include <float.h>
#include "../../include/minirt.h"
#include "../../include/scene.h"
#include "../../include/hit.h"

static int	inside_cyl_height(const t_cyl *cylinder, t_vec3 p, t_vec3 v)
{
	float	height_pos;

	height_pos = v3_dot(v3_sub(p, cylinder->center), v);
	if ((fabsf(height_pos) <= cylinder->vars.half_height))
		return (1);
	return (0);
}
/*
* Purpose: Check if a point lies within the cylinder's height bounds.
* Logic: Project (p - center) onto the axis; compare magnitude to half_height.
* Use: Called to validate if a ray-cylinder intersection is within the
	finite height.
*/

float	pick_valid_t(const t_cyl *cyl, t_ray r, float t1, float t2)
{
	float	tmin;
	float	tmax;
	t_vec3	p;

	tmin = t2;
	if (t1 > EPSILON)
		tmin = t1;
	tmax = t1;
	if (t2 > EPSILON)
		tmax = t2;
	if (tmin > EPSILON)
	{
		p = v3_add(r.orig, v3_mul(r.dir, tmin));
		if (inside_cyl_height(cyl, p, cyl->axis))
			return (tmin);
	}
	if (tmax > EPSILON && tmax >= tmin)
	{
		p = v3_add(r.orig, v3_mul(r.dir, tmax));
		if (inside_cyl_height(cyl, p, cyl->axis))
			return (tmax);
	}
	return (-1.0f);
}
/*
* Purpose: Select the nearest valid intersection on the cylinder's
	curved surface.
* Logic: Check both roots of the quadratic; return the first within
	height bounds.
* Notes: Prefers tmin (closer root); validates height with inside_cyl_height.
*/

float	check_best_t(float cyl_part, float best_t, t_cyl *cyl,
		int index)
{
	if (cyl_part > 0.0f && cyl_part < best_t)
	{
		cyl->vars.hit_part = index;
		return (cyl_part);
	}
	return (best_t);
}
