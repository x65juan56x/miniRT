#include <math.h>
#include "../../include/scene.h"

void	aux_sphere(t_sphere *sp)
{
	sp->vars.radius = sp->di * 0.5f;
	sp->vars.radius2 = sp->vars.radius * sp->vars.radius;
}
/*
* Purpose: Precompute auxiliary variables for sphere geometry.
* Logic: Calculate radius (half of diameter) and radius squared.
* Notes: radius2 is cached to avoid repeated multiplications in
	intersection tests.
* Use: Called once after parsing a sphere; optimizes hit detection.
*/

void	aux_plane(t_plane *pl)
{
	pl->normal = v3_norm(pl->normal);
	pl->vars.d = v3_dot(pl->normal, pl->point);
}
/*
* Purpose: Precompute auxiliary variables for plane geometry.
* Logic: Normalize the normal vector and compute the plane equation
	constant d.
* Notes: d = normal · point represents the plane's signed distance
	from origin.
* Use: Called once after parsing a plane; ensures normalized normal and
	fast intersection.
*/

void	aux_cylinder(t_cyl *cy)
{
	cy->axis = v3_norm(cy->axis);
	cy->vars.radius = cy->di * 0.5f;
	cy->vars.radius2 = cy->vars.radius * cy->vars.radius;
	cy->vars.half_height = cy->he * 0.5f;
	cy->vars.cap_top = v3_add(cy->center, v3_mul(cy->axis,
				cy->vars.half_height));
	cy->vars.cap_bottom = v3_sub(cy->center, v3_mul(cy->axis,
				cy->vars.half_height));
}
/*
* Purpose: Precompute auxiliary variables for cylinder geometry.
* Logic: Normalize axis; compute radius, radius squared, half height,
	and cap positions.
* Notes: cap_top and cap_bottom are the centers of the top and bottom disks.
*        radius2 and half_height are cached for efficient intersection tests.
* Use: Called once after parsing a cylinder; optimizes hit detection for
	side and caps.
*/
