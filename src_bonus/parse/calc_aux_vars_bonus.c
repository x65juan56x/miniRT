#include <math.h>
#include "../../include_bonus/minirt_bonus.h"

void	aux_sphere(t_sphere *sp)
{
	sp->vars.radius = sp->di * 0.5f;
	sp->vars.radius2 = sp->vars.radius * sp->vars.radius;
	sp->vars.inv_radius = 1.0f / sp->vars.radius;
}
/*
* Purpose: Precompute sphere geometry values from diameter for fast intersection tests.
* Algorithm:
*   - radius: Convert diameter to radius (divide by 2)
*   - radius2: Square of radius (avoids sqrt in intersection tests)
*   - inv_radius: Precompute 1/radius (avoids division during UV coordinate calculation)
* Use: Called after parsing sphere to optimize rendering calculations.
*/

void	aux_plane(t_plane *pl)
{
	t_vec3	up;

	pl->normal = v3_norm(pl->normal);
	pl->vars.d = v3_dot(pl->normal, pl->point);
	up = v3(0.0f, 1.0f, 0.0f);
	if (fabs(v3_dot(pl->normal, up)) > 0.999f)
		up = v3(1.0f, 0.0f, 0.0f);
	pl->vars.u = v3_norm(v3_cross(up, pl->normal));
	pl->vars.v = v3_cross(pl->normal, pl->vars.u);
}
/*
* Purpose: Prepare plane for intersection tests and build 2D texture coordinate system.
* Algorithm:
*   - normal: Normalize the plane normal vector
*   - d: Distance from origin (dot product of normal and point on plane)
*   - u, v: Build orthonormal basis in the plane for 2D coordinates
*       • Start with up vector (0,1,0) as reference
*       • If normal is nearly vertical (parallel to up), use (1,0,0) instead
*       • u: Cross product of up and normal (perpendicular to both)
*       • v: Cross product of normal and u (completes right-handed basis)
*   - This creates a local 2D coordinate system for checker patterns and textures
* Use: Called after parsing plane to enable intersection and surface effects.
*/

void	aux_cylinder(t_cyl *cy)
{
	t_vec3	up;

	cy->axis = v3_norm(cy->axis);
	cy->vars.radius = cy->di * 0.5f;
	cy->vars.radius2 = cy->vars.radius * cy->vars.radius;
	cy->vars.half_height = cy->he * 0.5f;
	cy->vars.cap_top = v3_add(cy->center, v3_mul(cy->axis,
				cy->vars.half_height));
	cy->vars.cap_bottom = v3_sub(cy->center, v3_mul(cy->axis,
				cy->vars.half_height));
	up = v3(0.0f, 1.0f, 0.0f);
	if (fabs(v3_dot(cy->axis, up)) > 0.999f)
		up = v3(1.0f, 0.0f, 0.0f);
	cy->vars.base_u = v3_norm(v3_cross(up, cy->axis));
	cy->vars.base_v = v3_cross(cy->axis, cy->vars.base_u);
}
/*
* Purpose: Precompute cylinder geometry and build coordinate system for caps and side.
* Algorithm:
*   - axis: Normalize the cylinder's axis direction
*   - radius, radius2: Convert diameter to radius and its square
*   - half_height: Half of total height (distance from center to each cap)
*   - cap_top: Position of top cap center (center + axis * half_height)
*   - cap_bottom: Position of bottom cap center (center - axis * half_height)
*   - base_u, base_v: Orthonormal basis perpendicular to axis
*       • Start with up vector (0,1,0) as reference
*       • If axis is nearly vertical, use (1,0,0) instead
*       • base_u: Cross product of up and axis
*       • base_v: Cross product of axis and base_u
*   - This 2D basis is used for cap texture mapping and checker patterns
* Use: Called after parsing cylinder to prepare for intersection and rendering.
*/

void	aux_triangle(t_triangle *tr)
{
	tr->vars.e1 = v3_sub(tr->b, tr->a);
	tr->vars.e2 = v3_sub(tr->c, tr->a);
	tr->vars.n = v3_cross(tr->vars.e1, tr->vars.e2);
	if (fabsf(tr->vars.n.x) > fabsf(tr->vars.n.y))
		tr->vars.tan = v3_norm(v3_cross(v3(0, 1, 0), tr->vars.n));
	else
		tr->vars.tan = v3_norm(v3_cross(v3(1, 0, 0), tr->vars.n));
	tr->vars.bit = v3_cross(tr->vars.n, tr->vars.tan);
	tr->vars.n = v3_norm(tr->vars.n);
	tr->vars.base_u = v3_norm(tr->vars.e1);
	tr->vars.base_v = v3_norm(v3_sub(tr->vars.e2,
				v3_mul(tr->vars.base_u, v3_dot(tr->vars.e2, tr->vars.base_u))));
}
/*
* Purpose: Precompute all geometric data needed for triangle intersection and shading.
* Algorithm:
*   - e1, e2: Edge vectors from vertex a to b and a to c (used for intersection tests)
*   - n: Surface normal computed via cross product (initially unnormalized to preserve area info)
*   - tan, bit: Tangent and bitangent vectors for texture/bump mapping
*       • Choose tangent perpendicular to normal, picking most stable axis
*       • Bitangent completes the orthogonal basis with normal and tangent
*   - n: Normalize the normal after tangent calculation
*   - base_u: Normalized direction along first edge (for 2D coordinate system)
*   - base_v: Orthogonalized second basis vector (Gram-Schmidt process)
*       • Projects e2 onto plane perpendicular to base_u for checker/texture mapping
* Use: Called after parsing triangle to prepare for rendering and surface effects.
*/

void	aux_hparab(t_hparab *hp)
{
	t_vec3	up;

	hp->axis = v3_norm(hp->axis);
	up = v3(0.0f, 1.0f, 0.0f);
	if (fabsf(v3_dot(hp->axis, up)) > 0.999f)
		up = v3(1.0f, 0.0f, 0.0f);
	hp->vars.u = v3_norm(v3_cross(up, hp->axis));
	hp->vars.v = v3_norm(v3_cross(hp->axis, hp->vars.u));
	hp->vars.rx2 = hp->rx * hp->rx;
	hp->vars.ry2 = hp->ry * hp->ry;
	hp->vars.inv_rx2 = 1.0f / hp->vars.rx2;
	hp->vars.inv_ry2 = 1.0f / hp->vars.ry2;
	hp->vars.half_height = hp->height * 0.5f;
	hp->vars.inv_height = 1.0f / hp->height;
}
/*
* Purpose: Precompute hyperbolic paraboloid (saddle shape) geometry for intersection.
* Algorithm:
*   - axis: Normalize the vertical axis direction
*   - u, v: Build orthonormal basis perpendicular to axis (same logic as cylinder/plane)
*       • Start with up vector (0,1,0), switch to (1,0,0) if axis is vertical
*       • u: Cross product of up and axis
*       • v: Cross product of axis and u
*   - rx2, ry2: Square of x and y radii (elliptical base dimensions)
*   - inv_rx2, inv_ry2: Precompute 1/(radius²) to avoid division in intersection equation
*   - half_height: Half of total height range
*   - inv_height: Precompute 1/height (used as scale factor in normal calculation)
* Notes: The paraboloid equation uses these values to define the saddle surface
*        z = (x²/rx² - y²/ry²) * height, which curves up in one direction and down in another.
* Use: Called after parsing paraboloid to prepare quadratic intersection equation.
*/
