#include <math.h>
#include "../../include/minirt.h"
#include "../../include/hit_bonus.h"

static void	hp_project(t_hparab *hp, t_ray r)
{
	t_vec3	rel;

	rel = v3_sub(r.orig, hp->center);
	hp->vars.ox = v3_dot(rel, hp->vars.u);
	hp->vars.oy = v3_dot(rel, hp->vars.v);
	hp->vars.oz = v3_dot(rel, hp->axis);
	hp->vars.dx = v3_dot(r.dir, hp->vars.u);
	hp->vars.dy = v3_dot(r.dir, hp->vars.v);
	hp->vars.dz = v3_dot(r.dir, hp->axis);
}

static void	hp_prepare(t_hparab *hp, t_ray r)
{
	hp_project(hp, r);
	hp->vars.a = hp->vars.dy * hp->vars.dy * hp->vars.inv_ry2
		- hp->vars.dx * hp->vars.dx * hp->vars.inv_rx2;
	hp->vars.b = 2.0f * hp->vars.oy * hp->vars.dy * hp->vars.inv_ry2
		- 2.0f * hp->vars.ox * hp->vars.dx * hp->vars.inv_rx2
		- hp->vars.dz * hp->vars.inv_height;
	hp->vars.c = hp->vars.oy * hp->vars.oy * hp->vars.inv_ry2
		- hp->vars.ox * hp->vars.ox * hp->vars.inv_rx2
		- hp->vars.oz * hp->vars.inv_height;
	hp->vars.disc = hp->vars.b * hp->vars.b - 4.0f * hp->vars.a * hp->vars.c;
}

static float	check_solution(const t_hparab *hp, int idx)
{
	float	x;
	float	y;
	float	z;
	float	inside;

	if (hp->vars.cands[idx] <= EPSILON)
		return (-1.0f);
	x = hp->vars.ox + hp->vars.dx * hp->vars.cands[idx];
	y = hp->vars.oy + hp->vars.dy * hp->vars.cands[idx];
	z = hp->vars.oz + hp->vars.dz * hp->vars.cands[idx];
	inside = x * x * hp->vars.inv_rx2 + y * y * hp->vars.inv_ry2;
	if (inside > 1.0f + 1e-4f)
		return (-1.0f);
	// Vertical clamp: allow full height (half_height set to height) with a small tolerance
	if (fabsf(z) > hp->vars.half_height + 2e-4f)
		return (-1.0f);
	return (hp->vars.cands[idx]);
}

static float	hp_solve_linear(const t_hparab *hp)
{
	float	t;

	if (fabsf(hp->vars.b) < 1e-8f)
		return (-1.0f);
	t = -hp->vars.c / hp->vars.b;
	return (check_solution(hp, t));
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
		t = check_solution(hp, idx);
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
	hp->vars.denom = 2.0f * hp->vars.a;
	hp->vars.cands[0] = (-hp->vars.b - sqrtf(hp->vars.disc)) / hp->vars.denom;
	hp->vars.cands[1] = (-hp->vars.b + sqrtf(hp->vars.disc)) / hp->vars.denom;
	return (hp_best_candidate(hp));
}
