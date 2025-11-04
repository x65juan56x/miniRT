#include <float.h>
#include "../../include_bonus/minirt_bonus.h"

void	cyl_quadratic(t_cyl *cyl, t_ray r, t_vec3 x)
{
	float	x_dot_ax;
	float	d_dot_ax;

	x_dot_ax = v3_dot(x, cyl->axis);
	d_dot_ax = v3_dot(r.dir, cyl->axis);
	cyl->vars.a = v3_dot(v3_sub(r.dir, v3_mul(cyl->axis, d_dot_ax)),
			v3_sub(r.dir, v3_mul(cyl->axis, d_dot_ax)));
	cyl->vars.b = 2.0f * v3_dot(v3_sub(r.dir, v3_mul(cyl->axis, d_dot_ax)),
			v3_sub(x, v3_mul(cyl->axis, x_dot_ax)));
	cyl->vars.c = (v3_dot(v3_sub(x, v3_mul(cyl->axis, x_dot_ax)),
				v3_sub(x, v3_mul(cyl->axis, x_dot_ax)))) - cyl->vars.radius2;
}
/*
* Purpose: Set up the quadratic equation to find where ray hits the curved side.
* Inputs: cyl (cylinder), ray r, x = vector from cylinder center to ray origin.
* Algorithm:
*   - We project everything onto the plane perpendicular to the cylinder's axis
*   - This simplifies the 3D problem: "does the ray hit the curved surface?"
*   - Builds coefficients a, b, c for the equation a*t² + b*t + c = 0
*   - These values get stored in cyl->vars for later solving
* Use: Called by hit_side() to prepare for intersection calculation.
*/

float	check_best_t(float cyl_part, float best_t, t_cyl *cyl, int index)
{
	if (cyl_part > 0.0f && cyl_part < best_t)
	{
		cyl->vars.hit_part = index;
		return (cyl_part);
	}
	return (best_t);
}
