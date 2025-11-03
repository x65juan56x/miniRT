#ifndef HIT_H
# define HIT_H

# include "vec3.h"
# include "ray.h"

// Forward declarations
struct	s_scene;
struct	s_sphere;
struct	s_plane;
struct	s_cyl;
// Generic hit record used by the renderer
typedef struct s_hit
{
	int		ok;
	float	t; // Distancia al primer objeto intersectado
	t_vec3	p; // Punto de impacto
	t_vec3	n; // Normal en el punto
	t_vec3	albedo; // Color del objeto intersectado
}	t_hit;

// Sphere auxiliaries (runtime temps + cached constants)
typedef struct s_sp_aux
{
	t_vec3	oc;
	float	radius;
	float	radius2;	// cached: radius^2
	float	a;
	float	half_b;
	float	c;
	float	disc;
}	t_sp_aux;

// Plane cached constants (computed once per object)
typedef struct s_pl_aux
{
	float	d;	// plane equation: dot(n, x) = d
}	t_pl_aux;

// Cylinder auxiliaries (runtime temps + cached constants)
typedef struct s_cy_aux
{
	float	radius;
	float	radius2;		// cached: radius^2
	float	half_height;	// he * 0.5
	t_vec3	cap_top;		// center + axis * half_height
	t_vec3	cap_bottom;		// center - axis * half_height
	float	a;
	float	b;
	float	c;
	int		hit_part;
}	t_cy_aux;

int		scene_hit(const struct s_scene *scene, t_ray r, float max_dist,
			t_hit *out);

/* HIT OBJECTS */
float	hit_sphere(struct s_sphere *sp, t_ray r);
float	hit_plane(struct s_plane *pl, t_ray r);
float	hit_cylinder(struct s_cyl *cy, t_ray r);

#endif
