#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/scene_bonus.h"
#include "../../include_bonus/hit_bonus.h"

float	hit_plane(t_plane *pl, t_ray r)
{
	float	den;
	float	t;

	den = v3_dot(pl->normal, r.dir);
	if (fabsf(den) < 1e-6f)
		return (-1.0f);
	t = (pl->vars.d - v3_dot(pl->normal, r.orig)) / den;
	if (t > 0.0f)
		return (t);
	return (-1.0f);
}
