#ifndef HIT_BONUS_H
# define HIT_BONUS_H

# include "../include/vec3.h"
# include "../include/ray.h"
# include "material_bonus.h"

// Forward declarations
struct	s_scene;
struct	s_sphere;
struct	s_plane;
struct	s_cyl;
struct	s_triangle;
struct	s_hparab;

typedef struct s_hit
{
	int				ok;
	float			t;	// Distance to the first intersected object
	t_vec3			p;	// Impact point
	t_vec3			n;	// Normal at the point
	t_vec3			albedo;	// Color of the intersected object
	float			ks;
	float			shininess;
	t_spec_model	spec_model;
	float			reflectivity;
	t_vec3			specular;
}	t_hit;

typedef struct s_sp_aux
{
	t_vec3	oc;
	float	radius;
	float	radius2; // cached: radius^2
	float	inv_radius; // cached: 1/radius (if needed)
	float	a;
	float	half_b;
	float	c;
	float	disc;
	t_vec3	u;
	t_vec3	v;
	// int		iu;
	// int		iv;
}	t_sp_aux;

// Plane cached constants (computed once per object)
typedef struct s_pl_aux
{
	float	d;	// plane equation: dot(n, x) = d
	t_vec3	u;	// tangent basis U (orthonormal to normal)
	t_vec3	v;	// tangent basis V (orthonormal to normal)
}	t_pl_aux;

typedef struct s_cy_aux
{
	float	radius;
	float	radius2;	// cached: radius^2
	float	half_height;	// he * 0.5
	t_vec3	cap_top;	// center + axis * half_height
	t_vec3	cap_bottom;	// center - axis * half_height
	t_vec3	base_u;	// tangent basis around axis (for UV/bump)
	t_vec3	base_v;	// bitangent basis around axis
	float	a;
	float	b;
	float	c;
	int		hit_part;
	int		cap_sign;
}	t_cy_aux;

typedef struct s_tr_aux
{
	t_vec3	e1;
	t_vec3	e2;
	t_vec3	n;	// cached normal (normalize(cross(e1,e2)))
	t_vec3	tan;	// tangent for UV/bump
	t_vec3	bit;	// bitangent for UV/bump
	t_vec3	pvec;
	float	det;
	float	inv_det;
	t_vec3	tvec;
	float	u;
	float	v;
	t_vec3	base_u;
	t_vec3	base_v;
	t_vec3	qvec;
}	t_tr_aux;

typedef struct s_hp_aux
{
	float	ox;
	float	oy;
	float	oz;
	float	dx;
	float	dy;
	float	dz;
	t_vec3	u;
	t_vec3	v;
	float	rx2;	// cached: rx^2
	float	ry2;	// cached: ry^2
	float	inv_rx2;	// cached: 1/(rx^2)
	float	inv_ry2;	// cached: 1/(ry^2)
	float	half_height;	// cached: height * 0.5
	float	inv_height;	// cached: 1/height
	float	a;
	float	b;
	float	c;
	float	disc;
	float	denom;
	float	cands[2];
}	t_hp_aux;

typedef struct s_cb_aux
{
	int		iu;
	int		iv;
	t_vec3	comp;
}	t_cb_aux;

typedef struct s_common_hit
{
	float	t;
	t_vec3	p;
	t_vec3	n;
	t_vec3	albedo;
}	t_common_hit;

int		scene_hit(const struct s_scene *scene, t_ray r,
			float max_dist, t_hit *out);

/* HIT OBJECTS */
float	hit_sphere(struct s_sphere *sp, t_ray r);
float	hit_plane(struct s_plane *pl, t_ray r);
float	hit_triangle(struct s_triangle *tr, t_ray r);
float	hit_cylinder(struct s_cyl *cy, t_ray r);
float	hit_hparaboloid(struct s_hparab *hp, t_ray r);

#endif
