#include <math.h>
#include "../../include/minirt.h"
#include "../../include/hit_bonus.h"

static void	hp_project(const t_hparab *hp, t_ray r, t_hp_aux *aux)
{
	t_vec3	rel;

	rel = v3_sub(r.orig, hp->center);
	aux->ox = v3_dot(rel, hp->u);
	aux->oy = v3_dot(rel, hp->v);
	aux->oz = v3_dot(rel, hp->axis);
	aux->dx = v3_dot(r.dir, hp->u);
	aux->dy = v3_dot(r.dir, hp->v);
	aux->dz = v3_dot(r.dir, hp->axis);
}

static void	hp_prepare(const t_hparab *hp, t_ray r, t_hp_aux *aux)
{
	hp_project(hp, r, aux);
	aux->a = aux->dy * aux->dy * hp->inv_ry2
		- aux->dx * aux->dx * hp->inv_rx2;
	aux->b = 2.0f * aux->oy * aux->dy * hp->inv_ry2
		- 2.0f * aux->ox * aux->dx * hp->inv_rx2
		- aux->dz * hp->inv_height;
	aux->c = aux->oy * aux->oy * hp->inv_ry2
		- aux->ox * aux->ox * hp->inv_rx2
		- aux->oz * hp->inv_height;
	aux->disc = aux->b * aux->b - 4.0f * aux->a * aux->c;
}

static float	check_solution(const t_hparab *hp, const t_hp_aux *aux, float t)
{
	float	x;
	float	y;
	float	z;
	float	inside;

	if (t <= EPSILON)
		return (-1.0f);
	x = aux->ox + aux->dx * t;
	y = aux->oy + aux->dy * t;
	z = aux->oz + aux->dz * t;
	inside = x * x * hp->inv_rx2 + y * y * hp->inv_ry2;
	if (inside > 1.0f + 1e-4f)
		return (-1.0f);
	// Vertical clamp: allow full height (half_height set to height) with a small tolerance
	if (fabsf(z) > hp->half_height + 2e-4f)
		return (-1.0f);
	return (t);
}

static float	hp_solve_linear(const t_hparab *hp, const t_hp_aux *aux)
{
	float	t;

	if (fabsf(aux->b) < 1e-8f)
		return (-1.0f);
	t = -aux->c / aux->b;
	return (check_solution(hp, aux, t));
}

static void	hp_store_candidates(t_hp_aux *aux)
{
	float	sqrt_disc;

	sqrt_disc = sqrtf(aux->disc);
	aux->denom = 2.0f * aux->a;
	aux->cands[0] = (-aux->b - sqrt_disc) / aux->denom;
	aux->cands[1] = (-aux->b + sqrt_disc) / aux->denom;
}

static float	hp_best_candidate(const t_hparab *hp, t_hp_aux *aux)
{
	float	best;
	int		idx;
	float	t;

	best = -1.0f;
	idx = 0;
	while (idx < 2)
	{
		t = check_solution(hp, aux, aux->cands[idx]);
		if (t > 0.0f && (best < 0.0f || t < best))
			best = t;
		idx++;
	}
	return (best);
}

float	hit_hparaboloid(const t_hparab *hp, t_ray r)
{
	t_hp_aux	aux;

	hp_prepare(hp, r, &aux);
	if (fabsf(aux.a) < 1e-8f)
		return (hp_solve_linear(hp, &aux));
	if (aux.disc < 0.0f)
		return (-1.0f);
	hp_store_candidates(&aux);
	return (hp_best_candidate(hp, &aux));
}

