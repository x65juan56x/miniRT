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
	// point must lie inside the elliptical cross-section
	inside = x * x * hp->vars.inv_rx2 + y * y * hp->vars.inv_ry2;
	if (inside > 1.0f + 1e-4f)
		return (-1.0f);
	// vertical clamp by half-height
	if (fabsf(z) > hp->height + 2e-4f)
		return (-1.0f);
	return (t);
}

static float	hp_solve_linear(const t_hparab *hp)
{
	float	t;

	if (fabsf(hp->vars.b) < 1e-8f)
		return (-1.0f);
	t = -hp->vars.c / hp->vars.b;
	return (check_solution(hp, t));
}

static void	hp_store_candidates(t_hp_aux *aux)
{
	float	sqrt_disc;

	sqrt_disc = sqrtf(aux->disc);
	aux->denom = 2.0f * aux->a;
	aux->cands[0] = (-aux->b - sqrt_disc) / aux->denom;
	aux->cands[1] = (-aux->b + sqrt_disc) / aux->denom;
}

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
