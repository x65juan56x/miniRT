#include <stdlib.h>
#include "vec3.h"
#include "scene.h"

void	scene_init(t_scene *s)
{
	s->ambient.ratio = 0.0f;
	s->ambient.color = v3(0, 0, 0);
	s->ambient.present = false;
	s->camera.pos = v3(0, 0, 0);
	s->camera.dir = v3(0, 0, -1);
	s->camera.fov_deg = 70.0f;
	s->camera.focal = 1.0f;
	s->camera.present = false;
	s->light.pos = v3(0, 0, 0);
	s->light.bright = 0.0f;
	s->light.color = v3(1, 1, 1);
	s->light.present = false;
	s->objects = NULL;
}
/*
* Purpose: Initialize the scene with defaults and presence flags set to false.
* Logic: Set ambient/camera/light defaults and an empty object list.
* Use: Call before parsing to ensure a consistent starting state.
*/

void	scene_free(t_scene *s)
{
	t_object	*it;
	t_object	*n;

	it = s->objects;
	while (it)
	{
		n = it->next;
		free(it);
		it = n;
	}
	s->objects = NULL;
}
/*
* Purpose: Free all scene objects and leave the scene in a clean state.
* Logic: Walk the linked list, free each node, and set objects = NULL.
*/

void	scene_add_object(t_scene *s, t_object *obj)
{
	obj->next = s->objects;
	s->objects = obj;
}
/*
* Purpose: Insert an object into the scene.
* Logic: Push-front insertion into the linked list.
* Notes: Order doesn't affect ray tracing; all intersections are tested.
*/
