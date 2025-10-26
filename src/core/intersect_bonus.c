#include "../../include/minirt.h"
#include "../../include/scene_bonus.h"
#include "../../include/hit_bonus.h"
#include "../../include/bump_bonus.h"

static void	set_common_hit(t_hit *dst, float t, t_vec3 p, t_vec3 n, t_vec3 albedo)
{
	dst->ok = 1;
	dst->t = t;
	dst->p = p;
	dst->n = n;
	dst->albedo = albedo;
}

static void	orient_normal(t_hit *hit, t_ray r)
{
	if (v3_dot(hit->n, r.dir) > 0.0f)
		hit->n = v3_mul(hit->n, -1.0f);
}

static int	record_sphere(const t_sphere *sp, t_ray r, float t, t_hit *out)
{
	t_vec3	p;
	t_vec3	n;
	float	uang;
	int		iu;
	int		iv;
	float	u;
	float	v;
	t_vec3	tan;
	t_vec3	bit;

	p = ray_at(r, t);
	n = v3_norm(v3_sub(p, sp->center));
	if (sp->has_checker)
	{
		uang = atan2f(n.z, n.x); // Baldosas aproximadas por arco en unidades de mundo (R * ángulo)
		if (uang < 0.0f)
			uang += 6.283185307179586f; // 2*pi
		iu = (int)floorf((uang * (sp->di * 0.5f)) / sp->checker_scale);
		iv = (int)floorf((acosf(n.y) * (sp->di * 0.5f)) / sp->checker_scale);
		if ((iu + iv) & 1)
			set_common_hit(out, t, p, n, v3_sub(v3(1.0f, 1.0f, 1.0f),
				sp->color));
		else
			set_common_hit(out, t, p, n, sp->color);
	}
	else
		set_common_hit(out, t, p, n, sp->color);
	// Apply bump after albedo selection
	if (sp->has_bump && sp->bump)
	{
		// UV from normal
		u = (atan2f(n.z, n.x) + (float)M_PI) / (2.0f * (float)M_PI);
		v = acosf(n.y) / (float)M_PI;
		// Tangent basis from normal
		tan = v3_cross(v3(0.0f, 1.0f, 0.0f), n);
		if (v3_len2(tan) < 1e-6f)
			tan = v3_cross(v3(1.0f, 0.0f, 0.0f), n);
		tan = v3_norm(tan);
		bit = v3_cross(n, tan);
		bump_perturb(sp->bump, u, v, tan, bit, sp->bump_strength, &out->n);
	}
	orient_normal(out, r);
	return (1);
}

static int	record_plane(const t_plane *pl, t_ray r, float t, t_hit *out)
{
	t_vec3	p;
	t_vec3	rel;
	int		ix;
	int		iy;
	t_vec3	comp;
	float	u;
	float	v;

	p = ray_at(r, t);
	if (pl->has_checker)
	{
		rel = v3_sub(p, pl->point);
		ix = (int)floorf(v3_dot(rel, pl->u) / pl->checker_scale);
		iy = (int)floorf(v3_dot(rel, pl->v) / pl->checker_scale);
		comp = v3(1.0f, 1.0f, 1.0f);
		comp = v3_sub(comp, pl->color); // color complementario
		if ((ix + iy) & 1)
			set_common_hit(out, t, p, pl->normal, comp);
		else
			set_common_hit(out, t, p, pl->normal, pl->color);
	}
	else
		set_common_hit(out, t, p, pl->normal, pl->color);
	if (pl->has_bump && pl->bump)
	{
		rel = v3_sub(p, pl->point);
		u = v3_dot(rel, pl->u) / (pl->checker_scale > 0.0f ? pl->checker_scale : 1.0f);
		v = v3_dot(rel, pl->v) / (pl->checker_scale > 0.0f ? pl->checker_scale : 1.0f);
		bump_perturb(pl->bump, u, v, pl->u, pl->v, pl->bump_strength, &out->n);
	}
	orient_normal(out, r);
	return (1);
}

static int	record_triangle(const t_triangle *tr, t_ray r, float t, t_hit *out)
{
    t_vec3	p;
    t_vec3	e1;
    t_vec3	e2;
    t_vec3	n;
    int		par;
    float	u;
    float	v;

    p = ray_at(r, t);
    e1 = v3_sub(tr->b, tr->a);
    e2 = v3_sub(tr->c, tr->a);
    n = v3_norm(v3_cross(e1, e2));
    if (tr->has_checker)
    {
        par = (int)floorf(v3_dot(v3_sub(p, tr->a), tr->u) / tr->checker_scale)
            + (int)floorf(v3_dot(v3_sub(p, tr->a), tr->v) / tr->checker_scale);
        if (par & 1)
            set_common_hit(out, t, p, n, v3_sub(v3(1.0f, 1.0f, 1.0f), tr->color));
        else
            set_common_hit(out, t, p, n, tr->color);
    }
    else
        set_common_hit(out, t, p, n, tr->color);
    // Bump: usar baricéntricas para estirar el mapa a todo el triángulo
    if (tr->has_bump && tr->bump)
    {
        t_vec3 pa = v3_sub(p, tr->a);
        float d00 = v3_dot(e1, e1);
        float d01 = v3_dot(e1, e2);
        float d11 = v3_dot(e2, e2);
        float d20 = v3_dot(pa, e1);
        float d21 = v3_dot(pa, e2);
        float denom = d00 * d11 - d01 * d01;
        if (fabsf(denom) > 1e-12f)
        {
            float vb = (d11 * d20 - d01 * d21) / denom;
            float wb = (d00 * d21 - d01 * d20) / denom;
            u = vb;        // (u,v) de textura = (vb, wb)
            v = wb;
            // Base tangente a partir de e1,e2
            t_vec3 tan = v3_norm(e1);
            t_vec3 bit = v3_norm(v3_cross(n, tan));
            bump_perturb(tr->bump, u, v, tan, bit, tr->bump_strength, &out->n);
        }
    }
    orient_normal(out, r);
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

static int	record_hparaboloid(const t_hparab *hp, t_ray r, float t, t_hit *out)
{
    t_vec3	p;
    float	x;
    float	y;
    t_vec3	grad_local;
    t_vec3	n;
    float	u;
    float	v;

    p = ray_at(r, t);
    x = v3_dot(v3_sub(p, hp->center), hp->u);
    y = v3_dot(v3_sub(p, hp->center), hp->v);
    grad_local = v3(-2.0f * x * hp->inv_rx2, 2.0f * y * hp->inv_ry2, -hp->inv_height);
    n = v3_add(v3_add(v3_mul(hp->u, grad_local.x), v3_mul(hp->v, grad_local.y)),
        v3_mul(hp->axis, grad_local.z));
    n = v3_norm(n);
    if (hp->has_checker)
        set_common_hit(out, t, p, n, hp_checker_color(hp, x, y));
    else
        set_common_hit(out, t, p, n, hp->color);
    // Bump: normalizar UV con rx,ry para cubrir la elipse una vez
    if (hp->has_bump && hp->bump)
    {
        u = (x / hp->rx) * 0.5f + 0.5f;
        v = (y / hp->ry) * 0.5f + 0.5f;
        bump_perturb(hp->bump, u, v, hp->u, hp->v, hp->bump_strength, &out->n);
    }
    orient_normal(out, r);
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

static int record_cylinder(const t_cyl *cy, t_ray r, float t, t_hit *out, int hit_part)
{
	t_vec3 p;
	t_vec3 n;
	float radius;
	float half_h;
	t_vec3 up;
	t_vec3 U;
	t_vec3 V;
	t_vec3 rel;
	float h;
	t_vec3 radial;
	float theta;
	int iu;
	int iv;
	t_vec3 comp;
	float u;
	float v;
	t_vec3 tan;
	t_vec3 bit;

	p = ray_at(r, t);
	radius = cy->di * 0.5f;
	half_h = cy->he * 0.5f;
	// Build an orthonormal basis (U,V) around the axis for angular/planar mapping
	up = v3(0.0f, 1.0f, 0.0f);
	if (fabsf(v3_dot(cy->axis, up)) > 0.999f)
		up = v3(1.0f, 0.0f, 0.0f);
	U = v3_norm(v3_cross(up, cy->axis));
	V = v3_cross(cy->axis, U);
	if (hit_part == 0)
	{
		// Side
		n = v3_norm(normal_cyl(cy, p));
		rel = v3_sub(p, cy->center);
		h = v3_dot(rel, cy->axis);
		radial = v3_norm(v3_sub(rel, v3_mul(cy->axis, h)));
		theta = atan2f(v3_dot(radial, V), v3_dot(radial, U));
		if (theta < 0.0f)
			theta += 6.283185307179586f; // 2*pi
		if (cy->has_checker)
		{
			iu = (int)floorf((radius * theta) / cy->checker_scale);
			iv = (int)floorf(((h + half_h)) / cy->checker_scale);
			comp = v3_sub(v3(1.0f, 1.0f, 1.0f), cy->color);
			if ((iu + iv) & 1)
				set_common_hit(out, t, p, n, comp);
			else
				set_common_hit(out, t, p, n, cy->color);
		}
		else
			set_common_hit(out, t, p, n, cy->color);
		// Bump on side: cylindrical UV in [0,1]x[0,1]
		if (cy->has_bump && cy->bump)
		{
			u = theta / (2.0f * (float)M_PI);
			v = (h + half_h) / cy->he;
			// Tangent basis: around azimuth and along axis
			tan = v3_norm(v3_cross(cy->axis, n));
			if (v3_len2(tan) < 1e-8f)
				tan = U; // fallback
			bit = cy->axis;
			bump_perturb(cy->bump, u, v, tan, bit, cy->bump_strength, &out->n);
		}
	}
	else if (hit_part == 1)
	{
		// Top cap
		n = cy->axis;
		t_vec3 ctop = v3_add(cy->center, v3_mul(cy->axis, half_h));
		t_vec3 q = v3_sub(p, ctop);
		if (cy->has_checker)
		{
			iu = (int)floorf(v3_dot(q, U) / cy->checker_scale);
			iv = (int)floorf(v3_dot(q, V) / cy->checker_scale);
			comp = v3_sub(v3(1.0f, 1.0f, 1.0f), cy->color);
			if ((iu + iv) & 1)
				set_common_hit(out, t, p, n, comp);
			else
				set_common_hit(out, t, p, n, cy->color);
		}
		else
			set_common_hit(out, t, p, n, cy->color);
		if (cy->has_bump && cy->bump)
		{
			float x = v3_dot(q, U);
			float y = v3_dot(q, V);
			// Normalize to [0,1] across the disk extent
			u = (x / radius) * 0.5f + 0.5f;
			v = (y / radius) * 0.5f + 0.5f;
			bump_perturb(cy->bump, u, v, U, V, cy->bump_strength, &out->n);
		}
	}
	else if (hit_part == 2)
	{
		// Bottom cap
		n = v3_mul(cy->axis, -1.0f);
		t_vec3 cbot = v3_sub(cy->center, v3_mul(cy->axis, half_h));
		t_vec3 q = v3_sub(p, cbot);
		if (cy->has_checker)
		{
			iu = (int)floorf(v3_dot(q, U) / cy->checker_scale);
			iv = (int)floorf(v3_dot(q, V) / cy->checker_scale);
			comp = v3_sub(v3(1.0f, 1.0f, 1.0f), cy->color);
			if ((iu + iv) & 1)
				set_common_hit(out, t, p, n, comp);
			else
				set_common_hit(out, t, p, n, cy->color);
		}
		else
			set_common_hit(out, t, p, n, cy->color);
		if (cy->has_bump && cy->bump)
		{
			float x = v3_dot(q, U);
			float y = v3_dot(q, V);
			u = (x / radius) * 0.5f + 0.5f;
			v = (y / radius) * 0.5f + 0.5f;
			bump_perturb(cy->bump, u, v, U, V, cy->bump_strength, &out->n);
		}
	}
	else
		return (0);
	orient_normal(out, r);
	return (1);
}

static int	object_hit(const t_object *obj, t_ray r, t_hit *out)
{
	float	t;
	int		hit_part;

	t = -1.0f;
	hit_part = -1;
	if (obj->type == OBJ_SPHERE)
		t = hit_sphere(&obj->u_obj.sp, r);
	else if (obj->type == OBJ_PLANE)
		t = hit_plane(&obj->u_obj.pl, r);
	else if (obj->type == OBJ_TRIANGLE)
		t = hit_triangle(&obj->u_obj.tr, r);
	else if (obj->type == OBJ_CYLINDER)
		t = hit_cylinder(&obj->u_obj.cy, r, &hit_part);
	else if (obj->type == OBJ_HPARABOLOID)
		t = hit_hparaboloid(&obj->u_obj.hp, r);
	if (t <= 0.0f)
		return (0);
	if (obj->type == OBJ_SPHERE)
		return (record_sphere(&obj->u_obj.sp, r, t, out));
	if (obj->type == OBJ_PLANE)
		return (record_plane(&obj->u_obj.pl, r, t, out));
	if (obj->type == OBJ_CYLINDER)
		return (record_cylinder(&obj->u_obj.cy, r, t, out, hit_part));
	if (obj->type == OBJ_HPARABOLOID)
		return (record_hparaboloid(&obj->u_obj.hp, r, t, out));
	return (record_triangle(&obj->u_obj.tr, r, t, out));
}

int	scene_hit(const t_scene *scene, t_ray r, float max_dist, t_hit *out)
{
	const t_object	*o;
	float			best;
	t_hit			cur;
	int				found;

	out->ok = 0;
	found = 0;
	best = max_dist;
	o = scene->objects;
	while (o)
	{
		if (object_hit(o, r, &cur) && cur.t > EPSILON && cur.t < best)
		{
			best = cur.t;
			*out = cur;
			found = 1;
		}
		o = o->next;
	}
	return (found);
}
