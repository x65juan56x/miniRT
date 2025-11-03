/*
* Scene model: ambient light, camera, light and object list used by miniRT.
* This header declares the core data structures produced by the parser and
* consumed by the renderer/shading code.
*/
#ifndef SCENE_BONUS_H
# define SCENE_BONUS_H

# include <stdbool.h>
# include <stdint.h>
# include "../include/vec3.h"
# include "bump_bonus.h"
# include "hit_bonus.h"

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
	t_vec3			pos;
	float			bright; // [0,1]
	t_vec3			color; // [0,1]
	bool			present;
	struct s_light	*next;
}	t_light;

/* Object kinds supported in the mandatory part. */
typedef enum e_objtype
{
	OBJ_SPHERE,
	OBJ_PLANE,
	OBJ_CYLINDER,
	OBJ_TRIANGLE,
	OBJ_HPARABOLOID
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
	int			has_checker;
	float		checker_scale;
	int			has_bump;
	float		bump_strength;
	t_bumpmap	*bump;
	t_material	*material;
	t_sp_aux	vars;
}	t_sphere;

typedef struct s_plane
{
	t_vec3		point;
	t_vec3		normal;
	t_vec3		color;
	int			has_checker;
	float		checker_scale;
	int			has_bump;
	float		bump_strength;
	t_bumpmap	*bump;
	t_material	*material;
	t_pl_aux	vars;
}	t_plane;

typedef struct s_cyl
{
	t_vec3		center;
	t_vec3		axis;
	float		di;
	float		he;
	t_vec3		color;
	int			has_checker;
	float		checker_scale;
	int			has_bump;
	float		bump_strength;
	t_bumpmap	*bump;
	t_material	*material;
	t_cy_aux	vars;
}	t_cyl;

/*
* Triangle primitive (non-mandatory; used by OBJ->RT tool and tests).
* a,b,c: triangle vertices in world space.
* color: albedo per channel in [0,1].
*/
typedef struct s_triangle
{
	t_vec3		a;
	t_vec3		b;
	t_vec3		c;
	t_vec3		color;
	int			has_checker;
	float		checker_scale;
	int			has_bump;
	float		bump_strength;
	t_bumpmap	*bump;
	t_material	*material;
	t_tr_aux	vars;
}	t_triangle;

typedef struct s_hparab
{
	t_vec3		center;
	t_vec3		axis;
	float		rx;
	float		ry;
	float		height;
	t_vec3		color;
	int			has_checker;
	float		checker_scale;
	int			has_bump;
	float		bump_strength;
	t_bumpmap	*bump;
	t_material	*material;
	t_hp_aux	vars;
}	t_hparab;

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
		t_triangle	tr;
		t_hparab	hp;
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
	t_light		*light;
	t_object	*objects;
	int			reflection_depth;
	bool		reflection_limit_set;
}	t_scene;

/* Initialize a scene with defaults and no objects. */
void	scene_init(t_scene *s);
/* Free all objects in the scene and leave it empty. */
void	scene_free(t_scene *s);
/* Push an object into the scene object list (O(1)). */
void	scene_add_object(t_scene *s, t_object *obj);

/* AUX OBJECT VARS */
void	aux_sphere(t_sphere *sp);
void	aux_plane(t_plane *pl);
void	aux_cylinder(t_cyl *cy);
void	aux_triangle(t_triangle *tr);
void	aux_hparab(t_hparab *hp);

/* FREE OBJECTS */
void	free_sphere(t_object *it);
void	free_plane(t_object *it);
void	free_cylinder(t_object *it);
void	free_triangle(t_object *it);
void	free_hparab(t_object *it);

/* OBJECTS UTILS */
void	cyl_quadratic(t_cyl *cyl, t_ray r, t_vec3 x);
float	check_best_t(float cyl_part, float best_t, t_cyl *cyl, int index);
void	hp_prepare(t_hparab *hp, t_ray r);

#endif
