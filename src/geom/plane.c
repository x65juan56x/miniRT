#include "../../include/minirt.h"
#include "../../include/hit.h"

float	hit_plane(const t_plane *pl, t_ray r)
{
	float	den;
	float	t;

	den = v3_dot(pl->normal, r.dir);
	if (fabsf(den) < 1e-6f)
		return (-1.0f);
	t = v3_dot(v3_sub(pl->point, r.orig), pl->normal) / den;
	if (t > 0.0f)
		return (t);
	return (-1.0f);
}
