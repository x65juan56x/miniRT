#ifndef HIT_BONUS_H
# define HIT_BONUS_H

# include "vec3.h"
# include "ray.h"
# include "scene_bonus.h"
# include "material_bonus.h"

typedef struct s_hit
{
	int		ok;
	float	t; // Distancia al primer objeto intersectado
	t_vec3	p; // Punto de impacto
	t_vec3	n; // Normal en el punto
	t_vec3	albedo; // Color del objeto intersectado
}	t_hit;

typedef struct s_sp_aux
{
	t_vec3	oc;
	float	radius;
	float	a;
	float	half_b;
	float	c;
	float	disc;
}	t_sp_aux;

typedef struct s_tr_aux
{
	t_vec3	e1;
	t_vec3	e2;
	t_vec3	pvec;
	float	det;
	float	inv_det;
	t_vec3	tvec;
	float	u;
	float	v;
	t_vec3	qvec;
}	t_tr_aux;

typedef struct s_hp_aux
{
	float		ox;
	float		oy;
	float		oz;
	float		dx;
	float		dy;
	float		dz;
	float		a;
	float		b;
	float		c;
	float		disc;
	float		denom;
	float		cands[2];
}	t_hp_aux;

int		scene_hit(const t_scene *scene, t_ray r, float max_dist, t_hit *out);

/* HIT OBJECTS */
float	hit_sphere(const t_sphere *sp, t_ray r);
float	hit_plane(const t_plane *pl, t_ray r);
float	hit_triangle(const t_triangle *tr, t_ray r);
float	hit_cylinder(const t_cyl *cy, t_ray r);
float	hit_hparaboloid(const t_hparab *hp, t_ray r);

#endif
