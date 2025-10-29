#include "../../include_bonus/minirt_bonus.h"

static void	set_common_hit(t_hit *dst, t_common_hit *c_hit)
{
	dst->ok = 1;
	dst->t = c_hit->t;
	dst->p = c_hit->p;
	dst->n = c_hit->n;
	dst->albedo = c_hit->albedo;
	dst->ks = 0.0f;
	dst->shininess = 0.0f;
	dst->specular = v3(0.0f, 0.0f, 0.0f);
}

static void	orient_normal(t_hit *hit, t_ray r)
{
	if (v3_dot(hit->n, r.dir) > 0.0f)
		hit->n = v3_mul(hit->n, -1.0f);
}

static void	apply_specular(t_hit *out, const t_material *material)
{
	out->specular = v3(0.0f, 0.0f, 0.0f);
	out->ks = 0.0f;
	out->shininess = 0.0f;
	if (!material)
		return ;
	out->ks = material->ks;
	out->shininess = material->shininess;
	if (material->ks <= 0.0f || material->shininess <= 0.0f)
	{
		out->ks = 0.0f;
		out->shininess = 0.0f;
	}
}

static void	sp_process_checker(t_sphere *sp, t_common_hit *c_hit, t_hit *out)
{
	float			uang;

	uang = atan2f(c_hit->n.z, c_hit->n.x); // Baldosas aproximadas por arco en unidades de mundo (R * ángulo)
	if (uang < 0.0f)
		uang += 6.283185307179586f; // 2*pi
	sp->vars.iu = (int)floorf((uang * sp->vars.radius) / sp->checker_scale);
	sp->vars.iv = (int)floorf((acosf(c_hit->n.y) * sp->vars.radius)
		/ sp->checker_scale);
	if ((sp->vars.iu + sp->vars.iv) & 1)
		c_hit->albedo = v3_sub(v3(1.0f, 1.0f, 1.0f), sp->color);
	else
		c_hit->albedo = sp->color;
	set_common_hit(out, c_hit);
}

static void	sp_process_bump(const t_sphere *sp, t_common_hit *c_hit, t_hit *out)
{
	t_aux_bump	bm_aux;

	
	// UV from normal
	bm_aux.u = (atan2f(c_hit->n.z, c_hit->n.x) + (float)M_PI) / (2.0f * (float)M_PI);
	bm_aux.v = acosf(c_hit->n.y) / (float)M_PI;
	// Tangent basis from normal
	bm_aux.tangent = v3_cross(v3(0.0f, 1.0f, 0.0f), c_hit->n);
	if (v3_len2(bm_aux.tangent) < 1e-6f)
		bm_aux.tangent = v3_cross(v3(1.0f, 0.0f, 0.0f), c_hit->n);
	bm_aux.tangent = v3_norm(bm_aux.tangent);
	bm_aux.bitangent = v3_cross(c_hit->n, bm_aux.tangent);
	bm_aux.strength = sp->bump_strength;
	bump_perturb(sp->bump, &bm_aux, &out->n);
}

static int	record_sphere(const t_scene *scene, t_sphere *sp, t_ray r,
		float t, t_hit *out)
{
	t_common_hit	c_hit;

	(void)scene;
	c_hit.p = ray_at(r, t);
	c_hit.n = v3_norm(v3_sub(c_hit.p, sp->center));
	c_hit.t = t;
	c_hit.albedo = sp->color;
	if (sp->has_checker)
		sp_process_checker(sp, &c_hit, out);
	else
		set_common_hit(out, &c_hit);
	if (sp->has_bump && sp->bump)
		sp_process_bump(sp, &c_hit, out);
	orient_normal(out, r);
	apply_specular(out, sp->material);
	return (1);
}
// Apply bump after albedo selection

static void	pl_process_checker(t_plane *pl, t_common_hit *c_hit, t_hit *out)
{
	t_vec3	rel;
	int		ix;
	int		iy;

	rel = v3_sub(c_hit->p, pl->point);
	ix = (int)floorf(v3_dot(rel, pl->vars.u) / pl->checker_scale);
	iy = (int)floorf(v3_dot(rel, pl->vars.v) / pl->checker_scale);
	if ((ix + iy) & 1)
		c_hit->albedo = v3_sub(v3(1.0f, 1.0f, 1.0f), pl->color); // color complementario
	set_common_hit(out, c_hit);
}

static void	pl_process_bump(const t_plane *pl, t_common_hit *c_hit, t_hit *out)
{
	t_aux_bump	bm_aux;
	t_vec3		rel;

	rel = v3_sub(c_hit->p, pl->point);
    bm_aux.u = v3_dot(rel, pl->vars.u)
        / (pl->checker_scale > 0.0f ? pl->checker_scale : 1.0f);
    bm_aux.v = v3_dot(rel, pl->vars.v)
        / (pl->checker_scale > 0.0f ? pl->checker_scale : 1.0f);
    bm_aux.tangent = pl->vars.u;
    bm_aux.bitangent = pl->vars.v;
	bm_aux.strength = pl->bump_strength;
	bump_perturb(pl->bump, &bm_aux, &out->n);
}

static int	record_plane(const t_scene *scene, t_plane *pl, t_ray r, float t,
			 t_hit *out)
{
	t_common_hit	c_hit;

	(void)scene;
	c_hit.p = ray_at(r, t);
	c_hit.t = t;
	c_hit.n = pl->normal;
	c_hit.albedo = pl->color;
	if (pl->has_checker)
		pl_process_checker(pl, &c_hit, out);
	else
		set_common_hit(out, &c_hit);
	if (pl->has_bump && pl->bump)
		pl_process_bump(pl, &c_hit, out);
	orient_normal(out, r);
	apply_specular(out, pl->material);
	return (1);
}

static void	tr_process_checker(t_triangle *tr, t_common_hit *c_hit, t_hit *out)
{
	int		par;

	par = (int)floorf(v3_dot(v3_sub(c_hit->p, tr->a), tr->vars.base_u)
		/ tr->checker_scale)
		+ (int)floorf(v3_dot(v3_sub(c_hit->p, tr->a), tr->vars.base_v)
		/ tr->checker_scale);
	if (par & 1)
		c_hit->albedo = v3_sub(v3(1.0f, 1.0f, 1.0f), tr->color);
	set_common_hit(out, c_hit);
}

static void	tr_process_bump(const t_triangle *tr, t_common_hit *c_hit, t_hit *out)
{
	t_aux_bump		bm_aux;
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
		bm_aux.u = vars.vb;        // (u,v) de textura = (vb, wb)
		bm_aux.v = vars.wb;
		// Base tangente a partir de e1,e2
		bm_aux.tangent = v3_norm(tr->vars.e1);
		bm_aux.bitangent = v3_norm(v3_cross(tr->vars.n, bm_aux.tangent));
		bump_perturb(tr->bump, &bm_aux, &out->n);
	}
}

static int	record_triangle(const t_scene *scene, t_triangle *tr, t_ray r,
		float t, t_hit *out)
{
	t_common_hit	c_hit;

	(void)scene;
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

static t_vec3	hp_checker_color(const t_hparab *hp, float x, float y)
{
	int	par;

	par = (int)floorf(x / hp->checker_scale)
		+ (int)floorf(y / hp->checker_scale);
	if (par & 1)
		return (v3_sub(v3(1.0f, 1.0f, 1.0f), hp->color));
	return (hp->color);
}

static void	hp_process_bump(const t_hparab *hp, float x, float y, t_hit *out)
{
	t_aux_bump	bm_aux;

	bm_aux.u = (x / hp->rx) * 0.5f + 0.5f;
	bm_aux.v = (y / hp->ry) * 0.5f + 0.5f;
	bm_aux.tangent = hp->vars.u;
	bm_aux.bitangent = hp->vars.v;
	bm_aux.strength = hp->bump_strength;
	bump_perturb(hp->bump, &bm_aux, &out->n);
}

static int	record_hparaboloid(const t_scene *scene, const t_hparab *hp,
		 t_ray r, float t, t_hit *out)
{
	t_common_hit	c_hit;
	float			x;
	float			y;

	(void)scene;
	c_hit.p = ray_at(r, t);
	c_hit.t = t;
	x = v3_dot(v3_sub(c_hit.p, hp->center), hp->vars.u);
	y = v3_dot(v3_sub(c_hit.p, hp->center), hp->vars.v);
	c_hit.n = v3_add(v3_add(v3_mul(hp->vars.u, -2.0f * x * hp->vars.inv_rx2),
		v3_mul(hp->vars.v, 2.0f * y * hp->vars.inv_ry2)),
		v3_mul(hp->axis, -hp->vars.inv_height));
	c_hit.n = v3_norm(c_hit.n);
	c_hit.albedo = hp->color;
	if (hp->has_checker)
		c_hit.albedo = hp_checker_color(hp, x, y);
	set_common_hit(out, &c_hit);
	if (hp->has_bump && hp->bump)
		hp_process_bump(hp, x, y, out);
	orient_normal(out, r);
	apply_specular(out, hp->material);
	return (1);
}

static t_vec3    normal_cyl(const t_cyl *cylinder, t_vec3 p)
{
	t_vec3  radial;
	float   axial;

	radial = v3_sub(p, cylinder->center);
	axial = v3_dot(radial, cylinder->axis);
	radial = v3_sub(radial, v3_mul(cylinder->axis, axial));
	return (radial);
}

static void	cy_hit_wall(const t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	t_aux_bump	bm_aux;
	t_vec3 rel;
	float h;
	t_vec3 radial;
	float theta;
	int iu;
	int iv;
	t_vec3 comp;
			// Side
	c_hit->n = v3_norm(normal_cyl(cy, c_hit->p));
	rel = v3_sub(c_hit->p, cy->center);
	h = v3_dot(rel, cy->axis);
	radial = v3_norm(v3_sub(rel, v3_mul(cy->axis, h)));
	theta = atan2f(v3_dot(radial, cy->vars.base_v), v3_dot(radial, cy->vars.base_u));
	if (theta < 0.0f)
		theta += 6.283185307179586f; // 2*pi
	if (cy->has_checker)
	{
		iu = (int)floorf((cy->vars.radius * theta) / cy->checker_scale);
		iv = (int)floorf(((h + cy->vars.half_height)) / cy->checker_scale);
		comp = v3_sub(v3(1.0f, 1.0f, 1.0f), cy->color);
		if ((iu + iv) & 1)
			c_hit->albedo = comp;
		set_common_hit(out, c_hit);
	}
	else
		set_common_hit(out, c_hit);
	// Bump on side: cylindrical UV in [0,1]x[0,1]
	if (cy->has_bump && cy->bump)
	{
		bm_aux.u = theta / (2.0f * (float)M_PI);
		bm_aux.v = (h + cy->vars.half_height) / cy->he;
		// Tangent basis: around azimuth and along axis
		bm_aux.tangent = v3_norm(v3_cross(cy->axis, c_hit->n));
		if (v3_len2(bm_aux.tangent) < 1e-8f)
			bm_aux.tangent = cy->vars.base_u; // fallback
		bm_aux.bitangent = cy->axis;
		bm_aux.strength = cy->bump_strength;
		bump_perturb(cy->bump, &bm_aux, &out->n);
	}
}

static void cy_hit_top(const t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	t_aux_bump	bm_aux;
	t_vec3	ctop;
	t_vec3	q;
	int		iu;
	int		iv;
	t_vec3	comp;
	float	x;
	float	y;

	c_hit->n = cy->axis;
	ctop = v3_add(cy->center, v3_mul(cy->axis, cy->vars.half_height));
	q = v3_sub(c_hit->p, ctop);
	if (cy->has_checker)
	{
		iu = (int)floorf(v3_dot(q, cy->vars.base_u) / cy->checker_scale);
		iv = (int)floorf(v3_dot(q, cy->vars.base_v) / cy->checker_scale);
		comp = v3_sub(v3(1.0f, 1.0f, 1.0f), cy->color);
		if ((iu + iv) & 1)
			c_hit->albedo = comp;
		set_common_hit(out, c_hit);
	}
	else
		set_common_hit(out, c_hit);
	if (cy->has_bump && cy->bump)
	{
		x = v3_dot(q, cy->vars.base_u);
		y = v3_dot(q, cy->vars.base_v);
		// Normalize to [0,1] across the disk extent
		bm_aux.u = (x / cy->vars.radius) * 0.5f + 0.5f;
		bm_aux.v = (y / cy->vars.radius) * 0.5f + 0.5f;
		bm_aux.tangent = cy->vars.base_u;
		bm_aux.bitangent = cy->vars.base_v;
		bm_aux.strength = cy->bump_strength;
		bump_perturb(cy->bump, &bm_aux, &out->n);
	}
}

static void cy_hit_bottom(const t_cyl *cy, t_common_hit *c_hit, t_hit *out)
{
	t_aux_bump	bm_aux;
	t_vec3	cbot;
	t_vec3	q;
	int		iu;
	int		iv;
	t_vec3	comp;
	float	x;
	float	y;

	c_hit->n = v3_mul(cy->axis, -1.0f);
	cbot = v3_sub(cy->center, v3_mul(cy->axis, cy->vars.half_height));
	q = v3_sub(c_hit->p, cbot);
	if (cy->has_checker)
	{
		iu = (int)floorf(v3_dot(q, cy->vars.base_u) / cy->checker_scale);
		iv = (int)floorf(v3_dot(q, cy->vars.base_v) / cy->checker_scale);
		comp = v3_sub(v3(1.0f, 1.0f, 1.0f), cy->color);
		if ((iu + iv) & 1)
			c_hit->albedo = comp;
		set_common_hit(out, c_hit);
	}
	else
		set_common_hit(out, c_hit);
	if (cy->has_bump && cy->bump)
	{
		x = v3_dot(q, cy->vars.base_u);
		y = v3_dot(q, cy->vars.base_v);
		// Normalize to [0,1] across the disk extent
		bm_aux.u = (x / cy->vars.radius) * 0.5f + 0.5f;
		bm_aux.v = (y / cy->vars.radius) * 0.5f + 0.5f;
		bm_aux.tangent = cy->vars.base_u;
		bm_aux.bitangent = cy->vars.base_v;
		bm_aux.strength = cy->bump_strength;
		bump_perturb(cy->bump, &bm_aux, &out->n);
	}
}

static int record_cylinder(const t_scene *scene, const t_cyl *cy, t_ray r,
		float t, t_hit *out)
{
	t_common_hit	c_hit;

	(void)scene;
	c_hit.p = ray_at(r, t);
	c_hit.t = t;
	c_hit.albedo = cy->color;
	if (cy->vars.hit_part == 0)
		cy_hit_wall(cy, &c_hit, out);
	else if (cy->vars.hit_part == 1)
		cy_hit_top(cy, &c_hit, out);
	else if (cy->vars.hit_part == 2)
		cy_hit_bottom(cy, &c_hit, out);
	else
		return (0);
	orient_normal(out, r);
	apply_specular(out, cy->material);
	return (1);
}

static int	object_hit(const t_scene *scene, t_object *obj, t_ray r, t_hit *out)
{
	float	t;
//	int		hit_part;

	t = -1.0f;
//	hit_part = -1;
	if (obj->type == OBJ_SPHERE)
		t = hit_sphere(&obj->u_obj.sp, r);
	else if (obj->type == OBJ_PLANE)
		t = hit_plane(&obj->u_obj.pl, r);
	else if (obj->type == OBJ_TRIANGLE)
		t = hit_triangle(&obj->u_obj.tr, r);
	else if (obj->type == OBJ_CYLINDER)
		t = hit_cylinder(&obj->u_obj.cy, r);
	else if (obj->type == OBJ_HPARABOLOID)
		t = hit_hparaboloid(&obj->u_obj.hp, r);
	if (t <= 0.0f)
		return (0);
	if (obj->type == OBJ_SPHERE)
		return (record_sphere(scene, &obj->u_obj.sp, r, t, out));
	if (obj->type == OBJ_PLANE)
		return (record_plane(scene, &obj->u_obj.pl, r, t, out));
	if (obj->type == OBJ_CYLINDER)
		return (record_cylinder(scene, &obj->u_obj.cy, r, t, out));
	if (obj->type == OBJ_HPARABOLOID)
		return (record_hparaboloid(scene, &obj->u_obj.hp, r, t, out));
	return (record_triangle(scene, &obj->u_obj.tr, r, t, out));
}

int	scene_hit(const t_scene *scene, t_ray r, float max_dist, t_hit *out)
{
	t_object	*o;
	float		best;
	t_hit		cur;
	int			found;

	out->ok = 0;
	found = 0;
	best = max_dist;
	o = scene->objects;
	while (o)
	{
		if (object_hit(scene, o, r, &cur) && cur.t > EPSILON && cur.t < best)
		{
			best = cur.t;
			*out = cur;
			found = 1;
		}
		o = o->next;
	}
	return (found);
}
