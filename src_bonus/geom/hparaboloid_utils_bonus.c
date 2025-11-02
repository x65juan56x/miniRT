#include <math.h>
#include "../../include_bonus/minirt_bonus.h"

static void	hp_project(t_hparab *hp, t_ray r)
{
	t_vec3	rel;

	rel = v3_sub(r.orig, hp->center);
	hp->vars.ox = v3_dot(rel, hp->vars.u);
	hp->vars.oy = v3_dot(rel, hp->vars.v);
	hp->vars.oz = v3_dot(rel, hp->axis);
	hp->vars.dx = v3_dot(r.dir, hp->vars.u);
	hp->vars.dy = v3_dot(r.dir, hp->vars.v);
	hp->vars.dz = v3_dot(r.dir, hp->axis);
}

void	hp_prepare(t_hparab *hp, t_ray r)
{
	hp_project(hp, r);
	hp->vars.a = hp->vars.dy * hp->vars.dy * hp->vars.inv_ry2
		- hp->vars.dx * hp->vars.dx * hp->vars.inv_rx2;
	hp->vars.b = 2.0f * hp->vars.oy * hp->vars.dy * hp->vars.inv_ry2
		- 2.0f * hp->vars.ox * hp->vars.dx * hp->vars.inv_rx2
		- hp->vars.dz * hp->vars.inv_height;
	hp->vars.c = hp->vars.oy * hp->vars.oy * hp->vars.inv_ry2
		- hp->vars.ox * hp->vars.ox * hp->vars.inv_rx2
		- hp->vars.oz * hp->vars.inv_height;
	hp->vars.disc = hp->vars.b * hp->vars.b - 4.0f * hp->vars.a * hp->vars.c;
}
