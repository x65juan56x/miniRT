#include "../../include/scene.h"

float	aux_sphere(t_sphere *sp)
{
	sp->vars.radius = sp->di * 0.5f;
	sp->vars.radius2 = sp->vars.radius * sp->vars.radius;
	sp->vars.inv_radius = 1.0f / sp->vars.radius;
}

float	aux_plane(t_plane *pl)
{
	t_vec3	up;

	pl->vars.d = v3_dot(pl->normal, pl->point);
	up = v3(0.0f, 1.0f, 0.0f);
	if (fabs(v3_dot(pl->normal, up)) > 0.999f)
		up = v3(1.0f, 0.0f, 0.0f);
	pl->vars.u = v3_norm(v3_cross(up, pl->normal));
	pl->vars.v = v3_cross(pl->normal, pl->vars.u);
}

float	aux_cylinder(t_cyl *cy)
{
	t_vec3	up;

	cy->vars.radius = cy->di * 0.5f;
	cy->vars.radius2 = cy->vars.radius * cy->vars.radius;
	cy->vars.half_height = cy->he * 0.5f;
	cy->vars.cap_top = v3_add(cy->center, v3_mul(cy->axis, cy->vars.half_height));
	cy->vars.cap_bottom = v3_sub(cy->center, v3_mul(cy->axis, cy->vars.half_height));
	up = v3(0.0f, 1.0f, 0.0f);
	if (fabs(v3_dot(cy->axis, up)) > 0.999f)
		up = v3(1.0f, 0.0f, 0.0f);
	cy->vars.base_u = v3_norm(v3_cross(up, cy->axis));
	cy->vars.base_v = v3_cross(cy->axis, cy->vars.base_u);
}
