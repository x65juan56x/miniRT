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
* Purpose: Apply a checkerboard pattern to the sphere surface using
*	spherical coordinates.
* Logic: Convert the surface normal (which points from center to hit point)
*	to spherical UV coordinates, then discretize into checker tiles.
* Math:
*	- normal: unit vector from sphere center to hit point
*	- uang = atan2(n.z, n.x): azimuthal angle φ in range [-π, π]
*	  (normalized to [0, 2π] by adding 2π if negative)
*	- polar angle θ = acos(n.y): angle from north pole [0, π]
*	- iu = floor(φ * radius / checker_scale): checker tile index (longitude)
*	- iv = floor(θ * radius / checker_scale): checker tile index (latitude)
* Notes: The arc length along the sphere surface is radius * angle, so
*	multiplying angles by radius gives world-space distances for uniform
*	tile sizing. Alternates between base color and complementary color
*	based on (iu + iv) parity.
* Use: Called by record_sphere when checker pattern is enabled.
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
* Purpose: Apply bump mapping to perturb the sphere's surface normal.
* Logic: Convert the surface normal to spherical UV coordinates [0,1]×[0,1]
*	and construct a tangent-space basis perpendicular to the normal.
* Math:
*	- normal: unit vector from sphere center to hit point
*	- u = (atan2(n.z, n.x) + π) / (2π): normalized azimuthal angle [0, 1]
*	  (adds π to shift from [-π, π] to [0, 2π], then normalizes)
*	- v = acos(n.y) / π: normalized polar angle [0, 1]
*	  (maps from [0, π] to [0, 1])
*	- tangent = normalize((0,1,0) × normal): tangent vector along longitude
*	  (if degenerate at poles, use (1,0,0) × normal as fallback)
*	- bitangent = normal × tangent: tangent vector along latitude
* Notes: The tangent basis (tangent, bitangent, normal) forms a right-handed
*	orthonormal coordinate system. At the north/south poles (n.y ≈ ±1),
*	the cross product with (0,1,0) degenerates, requiring a fallback.
* Use: Called by record_sphere when bump mapping is enabled; modifies out->n.
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
* Purpose: Record a ray-sphere intersection and compute all surface
*	properties for shading.
* Logic: Compute the hit point and outward normal, apply optional surface
*	effects (checker pattern and bump mapping), orient the normal to face
*	the ray, and apply material properties.
* Math:
*	- p = ray_origin + t * ray_direction: 3D hit point on the sphere
*	- normal = normalize(p - sphere_center): outward-pointing unit normal
*	  (for a sphere, the normal at any surface point is the radial direction
*	  from center to that point)
* Notes: The sphere normal is simply the normalized vector from the center
*	to the hit point. Surface effects are applied sequentially: checker
*	pattern (affects albedo) then bump mapping (perturbs normal). Finally,
*	orient_normal ensures the normal faces the ray origin for correct
*	two-sided rendering.
* Use: Called by the intersection dispatcher after hit_sphere confirms
*	a valid intersection distance t.
* Returns: Always returns 1 (success).
*/
