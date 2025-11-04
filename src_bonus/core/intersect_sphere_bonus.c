#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_internal_bonus.h"

static void	sp_process_checker(t_sphere *sp, t_common_hit *c_hit, t_hit *out)
{
	float		uang;
	t_cb_aux	vars;

	uang = atan2f(c_hit->n.z, c_hit->n.x);
	if (uang < 0.0f)
		uang += 6.283185307179586f;
	vars.iu = (int)floorf((uang * sp->vars.radius) / sp->checker_scale);
	vars.iv = (int)floorf((acosf(c_hit->n.y) * sp->vars.radius)
			/ sp->checker_scale);
	vars.comp = v3_sub(v3(1.0f, 1.0f, 1.0f), sp->color);
	if ((vars.iu + vars.iv) & 1)
		c_hit->albedo = vars.comp;
	else
		c_hit->albedo = sp->color;
	set_common_hit(out, c_hit);
}
/*
* Purpose: Apply a checkerboard pattern to the sphere's surface.
* Inputs: sp (sphere), c_hit (hit info with normal), out (final hit result).
* Algorithm:
*   - Think of the sphere like a globe with latitude and longitude lines
*   - Convert the surface normal to spherical coordinates (like GPS coordinates)
*   - uang: horizontal angle around the sphere (longitude)
*   - Vertical angle from top to bottom (latitude) using acos
*   - Divide these angles into tiles based on checker_scale
*   - Alternate between the base color and its complement like a checkerboard
* Use: Called when checker pattern is enabled on spheres.
*/

static void	sp_process_bump(const t_sphere *sp, t_common_hit *c_hit, t_hit *out)
{
	t_bump_aux	bm_aux;

	bm_aux.u = (atan2f(c_hit->n.z, c_hit->n.x)
			+ (float)M_PI) / (2.0f * (float)M_PI);
	bm_aux.v = acosf(c_hit->n.y) / (float)M_PI;
	bm_aux.tangent = v3_cross(v3(0.0f, 1.0f, 0.0f), c_hit->n);
	if (v3_len2(bm_aux.tangent) < 1e-6f)
		bm_aux.tangent = v3_cross(v3(1.0f, 0.0f, 0.0f), c_hit->n);
	bm_aux.tangent = v3_norm(bm_aux.tangent);
	bm_aux.bitangent = v3_cross(c_hit->n, bm_aux.tangent);
	bm_aux.strength = sp->bump_strength;
	bump_perturb(sp->bump, &bm_aux, &out->n);
}
/*
* Purpose: Apply bump mapping to make the sphere surface look textured.
* Inputs: sp (sphere with bump map), c_hit (hit info), out (result to modify).
* Algorithm:
*   - Convert the surface normal to UV coordinates [0,1] × [0,1]
*     (like unwrapping the sphere into a flat rectangle)
*   - u: horizontal coordinate (wraps around the sphere)
*   - v: vertical coordinate (from top pole to bottom pole)
*   - Build tangent vectors that lie flat on the sphere surface
*   - tangent: points along the "longitude" direction
*   - bitangent: points along the "latitude" direction
*   - Use these to perturb the normal based on the bump map texture
* Notes: At the north/south poles, we need a fallback tangent calculation
*        because the standard cross product becomes undefined.
* Use: Called when bump mapping is enabled; makes smooth spheres look bumpy.
*/

int	record_sphere(t_sphere *sp, t_ray r, float t, t_hit *out)
{
	t_common_hit	c_hit;

	c_hit.p = ray_at(r, t);
	c_hit.n = v3_norm(v3_sub(c_hit.p, sp->center));
	c_hit.t = t;
	c_hit.albedo = sp->color;
	if (sp->has_checker)
		sp_process_checker(sp, &c_hit, out);
	else
		set_common_hit(out, &c_hit);
	if (sp->has_bump && sp->bump)
		sp_process_bump(sp, &c_hit, out);
	orient_normal(out, r);
	apply_specular(out, sp->material);
	return (1);
}
/*
* Purpose: Process a confirmed sphere hit and prepare all surface information.
* Inputs: sp (sphere), ray r, t (hit distance), out (result structure to fill).
* Algorithm:
*   - Calculate where the ray hit the sphere (hit point p)
*   - Compute the surface normal (points outward from sphere center)
*   - Apply optional surface effects:
*       • Checker pattern: changes the color in a checkerboard
*       • Bump mapping: makes the surface look textured
*   - Orient the normal to face toward the ray (for proper lighting)
*   - Apply material properties (shininess, reflectivity, etc.)
* Notes: For a sphere, the normal is just the direction from center to hit point.
*        Surface effects are applied in order: first color, then bump, then orientation.
* Returns: Always returns 1 (success).
* Use: Called after hit_sphere() confirms there's an intersection.
*/
