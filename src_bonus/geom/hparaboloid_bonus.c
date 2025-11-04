#include <math.h>
#include "../../include_bonus/minirt_bonus.h"

static float	check_solution(const t_hparab *hp, float t)
{
	float	x;
	float	y;
	float	z;
	float	inside;

	if (t <= EPSILON)
		return (-1.0f);
	x = hp->vars.ox + hp->vars.dx * t;
	y = hp->vars.oy + hp->vars.dy * t;
	z = hp->vars.oz + hp->vars.dz * t;
	inside = x * x * hp->vars.inv_rx2 + y * y * hp->vars.inv_ry2;
	if (inside > 1.0f + 1e-4f)
		return (-1.0f);
	if (fabsf(z) > hp->height + 2e-4f)
		return (-1.0f);
	return (t);
}
/*
* Purpose: Verify if a potential hit point is actually on the paraboloid surface.
* Inputs: hp (hyperbolic paraboloid), t (distance along ray to check).
* Algorithm:
*   - Calculate where the ray would be at distance t (using stored coordinates)
*   - Check if that point lies within the elliptical cross-section boundary
*   - Check if the point is within the height limits (not above/below surface)
*   - Reject hits that are too close to ray origin (t <= EPSILON)
* Returns: t if valid hit, -1 if outside bounds or too close.
*/

static float	hp_solve_linear(const t_hparab *hp)
{
	float	t;

	if (fabsf(hp->vars.b) < 1e-8f)
		return (-1.0f);
	t = -hp->vars.c / hp->vars.b;
	return (check_solution(hp, t));
}
/*
* Purpose: Handle special case where the equation becomes linear (not quadratic).
* Inputs: hp (hyperbolic paraboloid with equation coefficients).
* Algorithm:
*   - This happens when coefficient 'a' is near zero
*   - Instead of a*t² + b*t + c = 0, we just have b*t + c = 0
*   - Solve directly: t = -c / b
*   - Check if this single solution is valid using check_solution()
* Returns: t if valid, -1 if invalid or b is too small (would divide by zero).
*/

static void	hp_store_candidates(t_hp_aux *aux)
{
	float	sqrt_disc;

	sqrt_disc = sqrtf(aux->disc);
	aux->denom = 2.0f * aux->a;
	aux->cands[0] = (-aux->b - sqrt_disc) / aux->denom;
	aux->cands[1] = (-aux->b + sqrt_disc) / aux->denom;
}
/*
* Purpose: Calculate the two possible hit distances from the quadratic formula.
* Inputs: aux (helper structure with equation coefficients a, b, discriminant).
* Algorithm:
*   - Compute sqrt of discriminant
*   - Use quadratic formula: t = (-b ± sqrt(disc)) / (2*a)
*   - Store both solutions (one with -, one with +) in the candidates array
* Use: Called when we have a proper quadratic equation with real solutions.
*/

static float	hp_best_candidate(const t_hparab *hp)
{
	float	best;
	int		idx;
	float	t;

	best = -1.0f;
	idx = 0;
	while (idx < 2)
	{
		t = check_solution(hp, hp->vars.cands[idx]);
		if (t > 0.0f && (best < 0.0f || t < best))
			best = t;
		idx++;
	}
	return (best);
}
/*
* Purpose: Pick the closest valid hit from the two candidate solutions.
* Inputs: hp (hyperbolic paraboloid with stored candidate distances).
* Algorithm:
*   - Test both candidates using check_solution() to see if they're valid
*   - Keep track of the nearest valid hit (smallest positive t)
*   - Prefer closer intersections when both are valid
* Returns: distance to nearest valid hit, or -1 if both candidates fail.
*/

float	hit_hparaboloid(t_hparab *hp, t_ray r)
{
	hp_prepare(hp, r);
	if (fabsf(hp->vars.a) < 1e-8f)
		return (hp_solve_linear(hp));
	if (hp->vars.disc < 0.0f)
		return (-1.0f);
	hp_store_candidates(&hp->vars);
	return (hp_best_candidate(hp));
}
/*
* Purpose: Find where a ray hits a hyperbolic paraboloid (saddle-shaped surface).
* Inputs: hp (hyperbolic paraboloid), ray r.
* Algorithm:
*   - First, prepare the equation by projecting ray into paraboloid's local space
*   - If coefficient 'a' ≈ 0, use linear solver (special case)
*   - disc (discriminant): tells us if ray hits the curved surface
*       • If negative → ray completely misses
*       • If zero or positive → ray might hit (1 or 2 intersection points)
*   - Calculate the two possible hit distances and pick the best valid one
*   - Each candidate is checked to ensure it's within the surface bounds
* Returns: distance to hit point, or -1 if no valid hit.
* Use: Main entry point for ray-paraboloid intersection tests.
*/
