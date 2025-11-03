#include "../../include/minirt.h"
#include "../../include/scene.h"
#include "../../include/hit.h"

void	orient_normal(t_hit *hit, t_ray r)
{
	if (v3_dot(hit->n, r.dir) > 0.0f)
		hit->n = v3_mul(hit->n, -1.0f);
}
/*
* Purpose: Ensure the normal points opposite to the ray direction.
* Logic: If normal and ray are in the same hemisphere, flip the normal.
* Use: Called after recording a hit to maintain consistent normal orientation.
*/

int	record_sphere(const t_sphere *sp, t_ray r, float t, t_hit *out)
{
	t_vec3	p;
	t_vec3	n;

	p = ray_at(r, t);
	n = v3_norm(v3_sub(p, sp->center));
	out->ok = 1;
	out->t = t;
	out->p = p;
	out->n = n;
	out->albedo = sp->color;
	orient_normal(out, r);
	return (1);
}
/*
* Purpose: Record sphere intersection data into the hit structure.
* Logic: Compute hit point, outward normal (p - center), and orient it.
* Notes: Normal is normalized; albedo is the sphere's surface color.
*/

int	record_plane(const t_plane *pl, t_ray r, float t, t_hit *out)
{
	t_vec3	p;

	p = ray_at(r, t);
	out->ok = 1;
	out->t = t;
	out->p = p;
	out->n = pl->normal;
	out->albedo = pl->color;
	orient_normal(out, r);
	return (1);
}
/*
* Purpose: Record plane intersection data into the hit structure.
* Logic: Compute hit point and use the plane's pre-defined normal.
* Notes: Plane normal is already normalized; orient based on ray direction.
*/

t_vec3	normal_cyl(const t_cyl *cylinder, t_vec3 p)
{
	t_vec3	radial;
	float	axial;

	radial = v3_sub(p, cylinder->center);
	axial = v3_dot(radial, cylinder->axis);
	radial = v3_sub(radial, v3_mul(cylinder->axis, axial));
	return (radial);
}
/*
* Purpose: Compute the radial component of a point on the cylinder surface.
* Logic: Project (p - center) onto the axis plane; result is perpendicular.
* Use: Called to get the outward normal for the cylinder's curved surface.
*/

int	record_cylinder(const t_cyl *cy, t_ray r, float t, t_hit *out)
{
	t_vec3	p;
	t_vec3	n;

	p = ray_at(r, t);
	if (cy->vars.hit_part == 0)
		n = v3_norm(normal_cyl(cy, p));
	else if (cy->vars.hit_part == 1)
		n = cy->axis;
	else if (cy->vars.hit_part == 2)
		n = v3_mul(cy->axis, -1);
	else
		return (0);
	out->ok = 1;
	out->t = t;
	out->p = p;
	out->n = n;
	out->albedo = cy->color;
	orient_normal(out, r);
	return (1);
}
/*
* Purpose: Record cylinder intersection data into the hit structure.
* Logic: Choose normal based on hit_part (0=side, 1=top cap, 2=bottom cap).
* Notes: Normalizes radial normal for curved surface; caps use axis ± direction.
*/
