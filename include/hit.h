#ifndef HIT_H
# define HIT_H

# include "vec3.h"
# include "ray.h"

# ifndef SCENE_HEADER
#  define SCENE_HEADER "scene.h"
# endif
# include SCENE_HEADER

typedef struct s_hit
{
	int		ok;
	float	t; // Distancia al primer objeto intersectado
	t_vec3	p; // Punto de impacto
	t_vec3	n; // Normal en el punto
	t_vec3	albedo; // Color del objeto intersectado
}   t_hit;

typedef struct s_sp_aux
{
	t_vec3	oc;
	float	radius;
	float	a;
	float	half_b;
	float	c;
	float	disc;
}   t_sp_aux;

int		scene_hit(const t_scene *scene, t_ray r, float max_dist, t_hit *out);

/* HIT OBJECTS */
float	hit_sphere(const t_sphere *sp, t_ray r);
float	hit_plane(const t_plane *pl, t_ray r);
float	hit_cylinder(const t_cyl *cy, t_ray r, int *hit_part);

#endif
