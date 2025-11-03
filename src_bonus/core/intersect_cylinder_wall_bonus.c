#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_internal_bonus.h"

static t_vec3	normal_cyl(const t_cyl *cylinder, t_vec3 p)
{
	t_vec3	radial;
	float	axial;

	radial = v3_sub(p, cylinder->center);
	axial = v3_dot(radial, cylinder->axis);
	radial = v3_sub(radial, v3_mul(cylinder->axis, axial));
	return (radial);
}
/*
* Purpose: Compute the outward-pointing radial normal vector at a point
*	on the cylinder's curved surface.
* Logic: Project the vector (p - center) onto the cylinder axis to find
*	the axial component, then subtract it to get the radial component.
* Math: radial = (p - center) - [(p - center) · axis] * axis
*	This removes the axial component, leaving only the perpendicular radial
*	vector that points from the cylinder axis to the surface point.
* Notes: The returned vector is unnormalized; caller must normalize if needed.
* Use: Called by cy_hit_wall to compute the surface normal for shading.
*/

static void	cywall_process_checker(t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	t_cb_aux	vars;
	t_vec3		rel;
	float		h;
	t_vec3		radial;
	float		theta;

	rel = v3_sub(c_hit->p, cy->center);
	h = v3_dot(rel, cy->axis);
	radial = v3_norm(v3_sub(rel, v3_mul(cy->axis, h)));
	theta = atan2f(v3_dot(radial, cy->vars.base_v),
			v3_dot(radial, cy->vars.base_u));
	if (theta < 0.0f)
		theta += 6.283185307179586f;
	vars.iu = (int)floorf((cy->vars.radius * theta) / cy->checker_scale);
	vars.iv = (int)floorf(((h + cy->vars.half_height)) / cy->checker_scale);
	vars.comp = v3_sub(v3(1.0f, 1.0f, 1.0f), cy->color);
	if ((vars.iu + vars.iv) & 1)
		c_hit->albedo = vars.comp;
	set_common_hit(out, c_hit);
}
/*
* Purpose: Apply a checkerboard pattern to the cylinder's curved surface.
* Logic: Convert 3D hit point to cylindrical UV coordinates (theta, h),
*	then discretize into checker tiles using checker_scale.
* Math:
*	- rel = p - center: vector from cylinder center to hit point
*	- h = rel · axis: axial position along the cylinder (height coordinate)
*	- radial = normalize(rel - (rel · axis) * axis): unit radial vector
*	- theta = atan2(radial · base_v, radial · base_u): azimuthal angle [0, 2π]
*	- iu = floor(radius * theta / checker_scale): checker tile index (azimuth)
*	- iv = floor((h + half_height) / checker_scale): checker tile index (height)
* Notes: Uses base_u and base_v as orthonormal basis vectors perpendicular
*	to the axis for computing the azimuthal angle. Alternates between
*	base color and complementary color based on (iu + iv) parity.
* Use: Called by cy_hit_wall when checker pattern is enabled.
*/

static void	cywall_process_bump(t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	t_bump_aux	bm_aux;
	t_vec3		rel;
	float		h;
	t_vec3		radial;
	float		theta;

	rel = v3_sub(c_hit->p, cy->center);
	h = v3_dot(rel, cy->axis);
	radial = v3_norm(v3_sub(rel, v3_mul(cy->axis, h)));
	theta = atan2f(v3_dot(radial, cy->vars.base_v),
			v3_dot(radial, cy->vars.base_u));
	if (theta < 0.0f)
		theta += 6.283185307179586f;
	bm_aux.u = theta / (2.0f * (float)M_PI);
	bm_aux.v = (h + cy->vars.half_height) / cy->he;
	bm_aux.tangent = v3_norm(v3_cross(cy->axis, c_hit->n));
	if (v3_len2(bm_aux.tangent) < 1e-8f)
		bm_aux.tangent = cy->vars.base_u;
	bm_aux.bitangent = cy->axis;
	bm_aux.strength = cy->bump_strength;
	bump_perturb(cy->bump, &bm_aux, &out->n);
}
/*
* Purpose: Apply bump mapping to perturb the surface normal on the
*	cylinder's curved wall.
* Logic: Compute cylindrical UV coordinates [0,1]×[0,1] and construct
*	a tangent-space basis for normal perturbation.
* Math:
*	- rel = p - center: vector from cylinder center to hit point
*	- h = rel · axis: axial position (height) along the cylinder
*	- radial = normalize(rel - (rel · axis) * axis): unit radial vector
*	- theta = atan2(radial · base_v, radial · base_u): azimuthal angle [0, 2π]
*	- u = theta / 2π: normalized azimuthal coordinate [0, 1]
*	- v = (h + half_height) / cylinder_height: normalized height [0, 1]
*	- tangent = normalize(axis × normal): tangent vector around the cylinder
*	- bitangent = axis: tangent vector along the cylinder's height
* Notes: The tangent basis (tangent, bitangent, normal) forms an orthonormal
*	coordinate system aligned with the cylinder's surface. The tangent wraps
*	around the azimuth, and the bitangent runs along the axis. Fallback to
*	base_u if cross product is degenerate (near-zero length).
* Use: Called by cy_hit_wall when bump mapping is enabled; modifies out->n.
*/

void	cy_hit_wall(t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	c_hit->n = v3_norm(normal_cyl(cy, c_hit->p));
	if (cy->has_checker)
		cywall_process_checker(cy, c_hit, out);
	else
		set_common_hit(out, c_hit);
	if (cy->has_bump && cy->bump)
		cywall_process_bump(cy, c_hit, out);
}
/*
* Purpose: Process a ray hit on the cylinder's curved wall surface,
*	computing normal and applying surface effects.
* Logic: Compute the radial surface normal, apply optional checkerboard
*	pattern, and perturb normal with bump mapping if enabled.
* Math: The normal is computed using the radial component of (p - center),
*	which is perpendicular to the cylinder axis and points outward from
*	the curved surface.
* Notes: This function handles only the cylindrical wall, not the caps.
*	Surface effects are applied sequentially: first checker pattern
*	(affects albedo), then bump mapping (perturbs normal).
* Use: Called by the intersection handler when a ray hits the curved
*	surface of a cylinder (hit_part == 0 or wall hit detected).
*/
