#include "../../include/minirt.h"
#include "../../include/scene.h"
#include "../../include/hit.h"

static int	object_hit(t_object *obj, t_ray r, t_hit *out)
{
	float	t;

	t = -1.0f;
	if (obj->type == OBJ_SPHERE)
		t = hit_sphere(&obj->u_obj.sp, r);
	else if (obj->type == OBJ_PLANE)
		t = hit_plane(&obj->u_obj.pl, r);
	else if (obj->type == OBJ_CYLINDER)
	{
		obj->u_obj.cy.vars.hit_part = -1;
		t = hit_cylinder(&obj->u_obj.cy, r);
	}
	if (t <= 0.0f)
		return (0);
	if (obj->type == OBJ_SPHERE)
		return (record_sphere(&obj->u_obj.sp, r, t, out));
	if (obj->type == OBJ_PLANE)
		return (record_plane(&obj->u_obj.pl, r, t, out));
	return (record_cylinder(&obj->u_obj.cy, r, t, out));
}
/*
* Purpose: Test if a ray hits an object and record the intersection.
* Logic: Call type-specific intersection function, then record if t > 0.
* Use: Dispatches to sphere/plane/cylinder handlers; returns 1 if hit found.
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
* Purpose: Find the nearest object hit by a ray within max_dist.
* Logic: Iterate all objects; keep the closest valid hit (t > EPSILON).
* Use: Returns 1 if any hit found; out contains the closest intersection.
* Notes: EPSILON avoids self-intersection artifacts; best tracks closest t.
*/
