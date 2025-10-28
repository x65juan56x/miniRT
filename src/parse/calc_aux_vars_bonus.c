#include <math.h>
#include "../../include/scene_bonus.h"

void	aux_sphere(t_sphere *sp)
{
	sp->vars.radius = sp->di * 0.5f;
	sp->vars.radius2 = sp->vars.radius * sp->vars.radius;
	sp->vars.inv_radius = 1.0f / sp->vars.radius;
	sp->vars.iu = 0;
	sp->vars.iv = 0;
}

void	aux_plane(t_plane *pl)
{
	t_vec3	up;

	pl->vars.d = v3_dot(pl->normal, pl->point);
	up = v3(0.0f, 1.0f, 0.0f);
	if (fabs(v3_dot(pl->normal, up)) > 0.999f)
		up = v3(1.0f, 0.0f, 0.0f);
	pl->vars.u = v3_norm(v3_cross(up, pl->normal));
	pl->vars.v = v3_cross(pl->normal, pl->vars.u);
}

void	aux_cylinder(t_cyl *cy)
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

void	aux_triangle(t_triangle *tr)
{
	// Edges
	tr->vars.e1 = v3_sub(tr->b, tr->a);
	tr->vars.e2 = v3_sub(tr->c, tr->a);
	// Normal (no normalizada para área)
	tr->vars.n = v3_cross(tr->vars.e1, tr->vars.e2);
	// Tangente y bitangente para UV/bump
	if (fabsf(tr->vars.n.x) > fabsf(tr->vars.n.y))
		tr->vars.tan = v3_norm(v3_cross(v3(0,1,0), tr->vars.n));
	else
		tr->vars.tan = v3_norm(v3_cross(v3(1,0,0), tr->vars.n));
	tr->vars.bit = v3_cross(tr->vars.n, tr->vars.tan);
	tr->vars.n = v3_norm(tr->vars.n);
}

void	aux_hparab(t_hparab *hp)
{
	t_vec3	up;

	up = v3(0.0f, 1.0f, 0.0f);
	if (fabsf(v3_dot(hp->axis, up)) > 0.999f)
		up = v3(1.0f, 0.0f, 0.0f);
	hp->vars.u = v3_norm(v3_cross(up, hp->axis));
	hp->vars.v = v3_norm(v3_cross(hp->axis, hp->vars.u));
	hp->vars.rx2 = hp->rx * hp->rx;
	hp->vars.ry2 = hp->ry * hp->ry;
	hp->vars.inv_rx2 = 1.0f / hp->vars.rx2;
	hp->vars.inv_ry2 = 1.0f / hp->vars.ry2;
	hp->vars.half_height = hp->height * 0.5f;
	hp->vars.inv_height = 1.0f / hp->height;
}
