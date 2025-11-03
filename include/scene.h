/*
* Scene model: ambient light, camera, light and object list used by miniRT.
* This header declares the core data structures produced by the parser and
* consumed by the renderer/shading code.
*/
#ifndef SCENE_H
# define SCENE_H

# include <stdbool.h>
# include <stdint.h>
# include "vec3.h"
# include "hit.h"

/*
* Ambient light declaration.
* ratio: strength in [0,1].
* color: per-channel color in [0,1].
* present: whether it was declared in the scene file.
*/
typedef struct s_ambient
{
	float	ratio; // [0,1]
	t_vec3	color; // [0,1] per channel
	bool	present; // true if declared in the .rt file
}	t_ambient;

/*
* Camera declaration.
* pos: camera position in world space.
* dir: view direction (must be normalized).
* fov_deg: vertical field of view in degrees, in (0,180).
* present: whether it was declared in the scene file.
*/
typedef struct s_camera
{
	t_vec3	pos;
	t_vec3	dir; // normalized
	float	fov_deg; // (0,180)
	bool	present;
	float	focal;
}	t_camera;

/*
* Point light declaration.
* pos: light position in world space.
* bright: brightness/intensity in [0,1].
* color: per-channel color in [0,1] (optional in subject,
* stored for extensibility).
* present: whether it was declared in the scene file.
*/
typedef struct s_light
{
	t_vec3	pos;
	float	bright; // [0,1]
	t_vec3	color; // [0,1]
	bool	present;
}	t_light;

/* Object kinds supported in the mandatory part. */
typedef enum e_objtype
{
	OBJ_SPHERE,
	OBJ_PLANE,
	OBJ_CYLINDER
}	t_objtype;

/*
* Object payloads.
* sphere: center, diameter (>0), and albedo color ([0,1] per channel).
* plane: point, normalized normal, and albedo color.
* cylinder: center, normalized axis, diameter (>0), height (>0), albedo color.
*/
typedef struct s_sphere
{
	t_vec3		center;
	float		di;
	t_vec3		color;
	t_sp_aux	vars;
}	t_sphere;

typedef struct s_plane
{
	t_vec3		point;
	t_vec3		normal;
	t_vec3		color;
	t_pl_aux	vars;
}	t_plane;

typedef struct s_cyl
{
	t_vec3		center;
	t_vec3		axis;
	float		di;
	float		he;
	t_vec3		color;
	t_cy_aux	vars;
}	t_cyl;

/*
* Scene object node (singly-linked list).
* type: which shape the union holds.
* u: shape data.
* next: link to next object.
*/
typedef struct s_object
{
	t_objtype		type;
	union
	{
		t_sphere	sp;
		t_plane		pl;
		t_cyl		cy;
	} u_obj;
	struct s_object	*next;
}	t_object;

/*
* Scene root object holding global entities and the object list.
*/
typedef struct s_scene
{
	t_ambient	ambient;
	t_camera	camera;
	t_light		light;
	t_object	*objects;
}	t_scene;

/* Initialize a scene with defaults and no objects. */
void	scene_init(t_scene *s);
/* Free all objects in the scene and leave it empty. */
void	scene_free(t_scene *s);
/* Push an object into the scene object list (O(1)). */
void	scene_add_object(t_scene *s, t_object *obj);

/* INTERSECT */
void	orient_normal(t_hit *hit, t_ray r);
int		record_sphere(const t_sphere *sp, t_ray r, float t, t_hit *out);
int		record_plane(const t_plane *pl, t_ray r, float t, t_hit *out);
t_vec3	normal_cyl(const t_cyl *cylinder, t_vec3 p);
int		record_cylinder(const t_cyl *cy, t_ray r, float t, t_hit *out);

/* AUX OBJECT VARS */
void	aux_sphere(t_sphere *sp);
void	aux_plane(t_plane *pl);
void	aux_cylinder(t_cyl *cy);

/* CYL_UTILS */
float	pick_valid_t(const t_cyl *cyl, t_ray r, float t1, float t2);
float	check_best_t(float cyl_part, float best_t, t_cyl *cyl, int index);

#endif
