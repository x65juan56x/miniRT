#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/intersect_internal_bonus.h"

static int	object_hit(t_object *obj, t_ray r, t_hit *out)
{
	float	t;

	t = -1.0f;
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
		return (record_sphere(&obj->u_obj.sp, r, t, out));
	if (obj->type == OBJ_PLANE)
		return (record_plane(&obj->u_obj.pl, r, t, out));
	if (obj->type == OBJ_CYLINDER)
		return (record_cylinder(&obj->u_obj.cy, r, t, out));
	if (obj->type == OBJ_HPARABOLOID)
		return (record_hparaboloid(&obj->u_obj.hp, r, t, out));
	return (record_triangle(&obj->u_obj.tr, r, t, out));
}
/*
* Purpose: Test if a ray hits a single object and record the hit details.
* Algorithm: First check the object type and test for intersection distance.
*            If hit is valid, call the appropriate record function to fill details.
* Returns: 1 if hit, 0 if miss.
*/

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
/*
* Purpose: Find the closest object hit by a ray in the entire scene.
* Algorithm: Loop through all objects, test each one, keep track of the nearest hit.
* Returns: 1 if any object was hit, 0 if ray missed everything.
*/
