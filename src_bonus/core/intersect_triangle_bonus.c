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
* Inputs: tr (triangle), c_hit (hit info), out (result structure).
* Algorithm:
*   - Calculate position relative to one corner of the triangle (vertex a)
*   - Project onto the triangle's local 2D coordinate system
*   - Divide into tiles based on checker_scale
*   - Alternate between base color and complement in a checkerboard
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
* Purpose: Apply bump mapping to make the flat triangle look textured.
* Inputs: tr (triangle with bump map), c_hit (hit info), out (result to modify).
* Algorithm:
*   - Calculate barycentric coordinates (how far along each triangle edge)
*   - These coordinates (vb, wb) become UV texture coordinates
*   - This stretches the bump map texture across the whole triangle
*   - Build tangent vectors from the triangle's edges
*   - Perturb the normal to create surface detail
* Notes: Uses barycentric coordinates to naturally map texture across the triangle.
*        If calculation fails (degenerate triangle), skip bump mapping.
* Use: Called when bump mapping is enabled on triangles.
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
	// Bump: usar baricéntricas para estirar el mapa a todo el triángulo
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
*   - Use the triangle's precomputed normal (same across the flat triangle)
*   - Apply optional surface effects:
*       • Checker pattern: changes color in a checkerboard
*       • Bump mapping: uses barycentric coords to map texture across triangle
*   - Orient the normal to face toward the ray
*   - Apply material properties (shininess, reflectivity, etc.)
* Notes: Triangles are flat, so they have a uniform normal like planes.
*        Bump mapping uses barycentric coordinates for natural texture mapping.
* Returns: Always returns 1 (success).
* Use: Called after hit_triangle() confirms there's an intersection.
*/
