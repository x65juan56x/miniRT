#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_internal_bonus.h"

static void	cycap_process_checker(t_cyl *cy, t_common_hit *c_hit, t_hit *out,
		t_vec3 q)
{
	t_cb_aux	vars;

	vars.iu = (int)floorf(v3_dot(q, cy->vars.base_u) / cy->checker_scale);
	vars.iv = (int)floorf(v3_dot(q, cy->vars.base_v) / cy->checker_scale);
	vars.comp = v3_sub(v3(1.0f, 1.0f, 1.0f), cy->color);
	if ((vars.iu + vars.iv) & 1)
		c_hit->albedo = vars.comp;
	set_common_hit(out, c_hit);
}
/*
* Purpose: Apply a checkerboard pattern to a cylinder cap (disk surface).
* Logic: Project the hit point onto the cap's local 2D coordinate system
*	using base_u and base_v, then discretize into checker tiles.
* Math:
*	- q: vector from cap center to hit point (already computed by caller)
*	- u = q · base_u: x-coordinate in the cap's local 2D plane
*	- v = q · base_v: y-coordinate in the cap's local 2D plane
*	- iu = floor(u / checker_scale): checker tile index along u-axis
*	- iv = floor(v / checker_scale): checker tile index along v-axis
* Notes: base_u and base_v form an orthonormal basis in the plane
*	perpendicular to the cylinder axis. The checkerboard alternates
*	between the base color and its complement based on (iu + iv) parity.
* Use: Called by cy_hit_top and cy_hit_bottom when checker pattern is enabled.
*/

static void	cycap_process_bump(t_cyl *cy, t_hit *out, t_vec3 q)
{
	t_bump_aux	bm_aux;
	float		x;
	float		y;

	x = v3_dot(q, cy->vars.base_u);
	y = v3_dot(q, cy->vars.base_v);
	bm_aux.u = (x / cy->vars.radius) * 0.5f + 0.5f;
	bm_aux.v = (y / cy->vars.radius) * 0.5f + 0.5f;
	bm_aux.tangent = cy->vars.base_u;
	bm_aux.bitangent = cy->vars.base_v;
	bm_aux.strength = cy->bump_strength;
	bump_perturb(cy->bump, &bm_aux, &out->n);
}
/*
* Purpose: Apply bump mapping to perturb the normal on a cylinder cap (disk).
* Logic: Convert the 3D cap hit point to normalized 2D UV coordinates [0,1]×[0,1]
*	and construct a tangent-space basis for normal perturbation.
* Math:
*	- q: vector from cap center to hit point
*	- x = q · base_u: position along the u-axis in cap's local plane
*	- y = q · base_v: position along the v-axis in cap's local plane
*	- u = (x / radius) * 0.5 + 0.5: normalized u-coordinate [0, 1]
*	- v = (y / radius) * 0.5 + 0.5: normalized v-coordinate [0, 1]
*	- tangent = base_u: tangent vector along the disk's u-axis
*	- bitangent = base_v: tangent vector along the disk's v-axis
* Notes: The UV mapping spans the entire disk diameter, normalized to [0,1].
*	The tangent basis (base_u, base_v, axis) forms an orthonormal coordinate
*	system. Division by radius and the transformation (x*0.5 + 0.5) maps the
*	range [-radius, radius] to [0, 1].
* Use: Called by cy_hit_top and cy_hit_bottom when bump mapping is enabled.
*/

void	cy_hit_top(t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	t_vec3		ctop;
	t_vec3		q;

	c_hit->n = cy->axis;
	ctop = v3_add(cy->center, v3_mul(cy->axis, cy->vars.half_height));
	q = v3_sub(c_hit->p, ctop);
	if (cy->has_checker)
		cycap_process_checker(cy, c_hit, out, q);
	else
		set_common_hit(out, c_hit);
	if (cy->has_bump && cy->bump)
		cycap_process_bump(cy, out, q);
}
/*
* Purpose: Process a ray hit on the cylinder's top cap, computing normal
*	and applying surface effects.
* Logic: Set the normal to the cylinder axis direction (pointing outward
*	from the top), compute the cap center position, and apply optional
*	checker pattern and bump mapping.
* Math:
*	- normal = axis: the top cap normal points in the same direction as the axis
*	- ctop = center + axis * half_height: position of the top cap center
*	- q = hit_point - ctop: vector from cap center to hit point
* Notes: The top cap is a circular disk perpendicular to the cylinder axis.
*	The normal always points in the axis direction. Vector q is used by
*	surface effect processors to determine local 2D coordinates on the disk.
* Use: Called by record_cylinder when hit_part == 1 (top cap hit).
*/

void	cy_hit_bottom(t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	t_vec3		cbot;
	t_vec3		q;

	c_hit->n = v3_mul(cy->axis, -1.0f);
	cbot = v3_sub(cy->center, v3_mul(cy->axis, cy->vars.half_height));
	q = v3_sub(c_hit->p, cbot);
	if (cy->has_checker)
		cycap_process_checker(cy, c_hit, out, q);
	else
		set_common_hit(out, c_hit);
	if (cy->has_bump && cy->bump)
		cycap_process_bump(cy, out, q);
}
/*
* Purpose: Process a ray hit on the cylinder's bottom cap, computing normal
*	and applying surface effects.
* Logic: Set the normal opposite to the cylinder axis direction (pointing
*	outward from the bottom), compute the cap center position, and apply
*	optional checker pattern and bump mapping.
* Math:
*	- normal = -axis: the bottom cap normal points opposite to the axis direction
*	- cbot = center - axis * half_height: position of the bottom cap center
*	- q = hit_point - cbot: vector from cap center to hit point
* Notes: The bottom cap is a circular disk perpendicular to the cylinder axis.
*	The normal points in the negative axis direction to face outward from
*	the cylinder. Vector q is used by surface effect processors to determine
*	local 2D coordinates on the disk.
* Use: Called by record_cylinder when hit_part == 2 (bottom cap hit).
*/
