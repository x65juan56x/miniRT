#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_internal_bonus.h"

static void	tr_process_checker(t_triangle *tr, t_common_hit *c_hit, t_hit *out)
{
	t_cb_aux	vars;

	vars.iu = (int)floorf(v3_dot(v3_sub(c_hit->p, tr->a), tr->vars.base_u)
			/ tr->checker_scale);
	vars.iv = (int)floorf(v3_dot(v3_sub(c_hit->p, tr->a), tr->vars.base_v)
			/ tr->checker_scale);
	vars.comp = v3_sub(v3(1.0f, 1.0f, 1.0f), tr->color);
	if ((vars.iu + vars.iv) & 1)
		c_hit->albedo = vars.comp;
	set_common_hit(out, c_hit);
}
/*
* Purpose: Apply a checkerboard pattern to the triangle's surface.
* Inputs: tr (triangle), c_hit (hit info with position), out (final hit result).
* Algorithm:
*   - Project the hit point onto the triangle's 2D coordinate system
*   - base_u and base_v are orthogonal vectors lying on the triangle plane
*   - Calculate position relative to vertex 'a' using dot products
*   - Divide these coordinates into tiles based on checker_scale
*   - Alternate between the base color and its complement like a checkerboard
* Notes: Unlike spheres, triangles use a flat 2D grid projection.
*        The grid is aligned with the triangle's edges via base_u and base_v.
* Use: Called when checker pattern is enabled on triangles.
*/

static void	tr_process_bump(const t_triangle *tr, t_common_hit *c_hit,
		t_hit *out)
{
	t_bump_aux		bm_aux;
	t_tr_bump_aux	vars;

	vars.pa = v3_sub(c_hit->p, tr->a);
	vars.d00 = v3_dot(tr->vars.e1, tr->vars.e1);
	vars.d01 = v3_dot(tr->vars.e1, tr->vars.e2);
	vars.d11 = v3_dot(tr->vars.e2, tr->vars.e2);
	vars.d20 = v3_dot(vars.pa, tr->vars.e1);
	vars.d21 = v3_dot(vars.pa, tr->vars.e2);
	vars.denom = vars.d00 * vars.d11 - vars.d01 * vars.d01;
	if (fabsf(vars.denom) > 1e-12f)
	{
		vars.vb = (vars.d11 * vars.d20 - vars.d01 * vars.d21) / vars.denom;
		vars.wb = (vars.d00 * vars.d21 - vars.d01 * vars.d20) / vars.denom;
		bm_aux.u = vars.vb;
		bm_aux.v = vars.wb;
		bm_aux.tangent = v3_norm(tr->vars.e1);
		bm_aux.bitangent = v3_norm(v3_cross(tr->vars.n, bm_aux.tangent));
		bump_perturb(tr->bump, &bm_aux, &out->n);
	}
}
/*
* Purpose: Apply bump mapping to make the triangle surface look textured.
* Inputs: tr (triangle with bump map), c_hit (hit info), out (result to modify).
* Algorithm:
*   - Compute barycentric coordinates (vb, wb) of the hit point on the triangle
*     (like finding how much "weight" each vertex has at this position)
*   - e1 and e2 are the triangle's edge vectors from vertex a
*   - Use dot products to solve for the barycentric coordinates
*   - Map barycentric coordinates to UV texture space [0,1] × [0,1]
*   - Build tangent space vectors aligned with the triangle's edges:
*       • tangent: points along e1 (first edge)
*       • bitangent: perpendicular to tangent, lies on triangle surface
*   - Use these to perturb the normal based on the bump map texture
* Notes: The barycentric calculation stretches the bump map across the entire triangle.
*        We check denom to avoid division by zero in degenerate triangles.
* Use: Called when bump mapping is enabled; makes flat triangles look textured.
*/

int	record_triangle(t_triangle *tr, t_ray r, float t, t_hit *out)
{
	t_common_hit	c_hit;

	c_hit.p = ray_at(r, t);
	c_hit.n = tr->vars.n;
	c_hit.t = t;
	c_hit.albedo = tr->color;
	if (tr->has_checker)
		tr_process_checker(tr, &c_hit, out);
	else
		set_common_hit(out, &c_hit);
	if (tr->has_bump && tr->bump)
		tr_process_bump(tr, &c_hit, out);
	orient_normal(out, r);
	apply_specular(out, tr->material);
	return (1);
}
/*
* Purpose: Process a confirmed triangle hit and prepare all surface information.
* Inputs: tr (triangle), ray r, t (hit distance), out (result structure to fill).
* Algorithm:
*   - Calculate where the ray hit the triangle (hit point p)
*   - Use the triangle's precomputed normal (same across entire surface)
*   - Apply optional surface effects:
*       • Checker pattern: changes the color in a checkerboard
*       • Bump mapping: makes the surface look textured
*   - Orient the normal to face toward the ray (for proper lighting)
*   - Apply material properties (shininess, reflectivity, etc.)
* Notes: Unlike spheres, triangles have a flat surface so the normal is constant.
*        Surface effects are applied in order: first color, then bump, then orientation.
* Returns: Always returns 1 (success).
* Use: Called after hit_triangle() confirms there's an intersection.
*/
