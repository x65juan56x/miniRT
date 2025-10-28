static void	hp_process_bump(const t_hparab *hp, float x, float y, t_hit *out)
{
	float	u;
	float	v;

	u = (x / hp->rx) * 0.5f + 0.5f;
	v = (y / hp->ry) * 0.5f + 0.5f;
	bump_perturb(hp->bump, u, v, &out->n);
}

static int	record_hparaboloid(const t_hparab *hp, t_ray r, float t, t_hit *out)
{
	t_vec3		p;
	float		x;
	float		y;
//	t_vec3		grad_local;
	t_vec3		n;
	float		u;
	float		v;

	p = ray_at(r, t);
	x = v3_dot(v3_sub(p, hp->center), hp->vars.u);
	y = v3_dot(v3_sub(p, hp->center), hp->vars.v);
//	grad_local = v3(-2.0f * x * hp->vars.inv_rx2, 2.0f * y * hp->vars.inv_ry2, -hp->vars.inv_height);
	n = v3_add(v3_add(v3_mul(hp->vars.u, -2.0f * x * hp->vars.inv_rx2),
		v3_mul(hp->vars.v, 2.0f * y * hp->vars.inv_ry2)),
		v3_mul(hp->axis, -hp->vars.inv_height));
	n = v3_norm(n);
	if (hp->has_checker)
		set_common_hit(out, t, p, n, hp_checker_color(hp, x, y));
	else
		set_common_hit(out, t, p, n, hp->color);
	// Bump: normalizar UV con rx,ry para cubrir la elipse una vez
	if (hp->has_bump && hp->bump)
		hp_process_bump(hp, x, y, out);
	orient_normal(out, r);
	return (1);
}