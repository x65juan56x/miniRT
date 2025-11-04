#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_internal_bonus.h"

void	pl_process_checker(t_plane *pl, t_common_hit *c_hit, t_hit *out)
{
	t_vec3		rel;
	t_cb_aux	vars;

	rel = v3_sub(c_hit->p, pl->point);
	vars.iu = (int)floorf(v3_dot(rel, pl->vars.u) / pl->checker_scale);
	vars.iv = (int)floorf(v3_dot(rel, pl->vars.v) / pl->checker_scale);
	vars.comp = v3_sub(v3(1.0f, 1.0f, 1.0f), pl->color);
	if ((vars.iu + vars.iv) & 1)
		c_hit->albedo = vars.comp;
	set_common_hit(out, c_hit);
}
/*
* Purpose: Apply a checkerboard pattern to the plane's surface.
* Inputs: pl (plane), c_hit (hit info), out (result structure).
* Algorithm:
*   - Calculate position relative to a reference point on the plane
*   - Project this position onto the plane's 2D coordinate system (u, v axes)
*   - Divide into tiles based on checker_scale
*   - Alternate between base color and complement color in a checkerboard
* Use: Called when checker pattern is enabled on planes.
*/

void	pl_process_bump(const t_plane *pl, t_common_hit *c_hit, t_hit *out)
{
	t_bump_aux	bm_aux;
	t_vec3		rel;

	rel = v3_sub(c_hit->p, pl->point);
	if (pl->checker_scale > 0.0f)
		bm_aux.u = v3_dot(rel, pl->vars.u) / pl->checker_scale;
	else
		bm_aux.u = v3_dot(rel, pl->vars.u);
	if (pl->checker_scale > 0.0f)
		bm_aux.v = v3_dot(rel, pl->vars.v) / pl->checker_scale;
	else
		bm_aux.v = v3_dot(rel, pl->vars.v);
	bm_aux.tangent = pl->vars.u;
	bm_aux.bitangent = pl->vars.v;
	bm_aux.strength = pl->bump_strength;
	bump_perturb(pl->bump, &bm_aux, &out->n);
}
/*
* Purpose: Apply bump mapping to make the flat plane look textured.
* Inputs: pl (plane with bump map), c_hit (hit info), out (result to modify).
* Algorithm:
*   - Find position relative to a reference point on the plane
*   - Convert to UV coordinates using the plane's local coordinate system
*   - If checker_scale exists, use it to tile the bump map
*   - Use the plane's u and v axes as tangent directions
*   - Perturb the normal based on the bump map to create surface detail
* Use: Called when bump mapping is enabled; makes flat planes look bumpy.
*/

int	record_plane(t_plane *pl, t_ray r, float t, t_hit *out)
{
	t_common_hit	c_hit;

	c_hit.p = ray_at(r, t);
	c_hit.t = t;
	c_hit.n = pl->normal;
	c_hit.albedo = pl->color;
	if (pl->has_checker)
		pl_process_checker(pl, &c_hit, out);
	else
		set_common_hit(out, &c_hit);
	if (pl->has_bump && pl->bump)
		pl_process_bump(pl, &c_hit, out);
	orient_normal(out, r);
	apply_specular(out, pl->material);
	return (1);
}
/*
* Purpose: Process a confirmed plane hit and prepare all surface information.
* Inputs: pl (plane), ray r, t (hit distance), out (result structure to fill).
* Algorithm:
*   - Calculate where the ray hit the plane (hit point p)
*   - Use the plane's constant normal (same everywhere on the plane)
*   - Apply optional surface effects:
*       • Checker pattern: changes color in a checkerboard
*       • Bump mapping: makes the surface look textured
*   - Orient the normal to face toward the ray
*   - Apply material properties (shininess, reflectivity, etc.)
* Notes: Planes have a uniform normal everywhere, unlike curved surfaces.
*        Surface effects are applied in order: color, bump, orientation.
* Returns: Always returns 1 (success).
* Use: Called after hit_plane() confirms there's an intersection.
*/
