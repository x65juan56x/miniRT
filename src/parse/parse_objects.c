#include <math.h>
#include <stdlib.h>
#include "../../include/parser_internal.h"

static t_parse_result	object_error(t_object *obj, int line, const char *msg)
{
	if (obj)
		free(obj);
	return (parse_error(line, msg));
}
/*
* Purpose: Wrap error reporting while safely releasing a partially built object.
* Inputs: Pointer to the allocated object, line number, and error string.
* Notes: Ensures no allocation leaks when validation fails mid-parsing.
*/

t_parse_result	parse_sp(char **tokens, int line, t_scene *scene)
{
	t_object	*obj;

	if (!tokens[1] || !tokens[2] || !tokens[3] || tokens[4])
		return (parse_error(line, "sp: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "sp: not enough memory"));
	obj->type = OBJ_SPHERE;
	if (!parse_vec3(tokens[1], &obj->u_obj.sp.center))
		return (object_error(obj, line, "sp: invalid centre"));
	if (!parse_float(tokens[2], &obj->u_obj.sp.di) || obj->u_obj.sp.di <= 0.0f)
		return (object_error(obj, line, "sp: invalid diameter"));
	if (!parse_color_255(tokens[3], &obj->u_obj.sp.color))
		return (object_error(obj, line, "sp: invalid color"));
	obj->next = NULL;
	scene_add_object(scene, obj);
	return (parse_ok());
}
/*
* Purpose: Parse a sphere entry, validating format, geometry, and color.
* Steps: Allocate, decode center/diameter/color, and insert into the scene.
* Errors: Frees the object and returns a detailed message on failure.
*/

t_parse_result	parse_pl(char **tokens, int line, t_scene *scene)
{
	t_object	*obj;

	if (!tokens[1] || !tokens[2] || !tokens[3] || tokens[4])
		return (parse_error(line, "pl: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "pl: not enough memory"));
	obj->type = OBJ_PLANE;
	if (!parse_vec3(tokens[1], &obj->u_obj.pl.point))
		return (object_error(obj, line, "pl: invalid point"));
	if (!parse_vec3(tokens[2], &obj->u_obj.pl.normal))
		return (object_error(obj, line, "pl: invalid normal"));
	if (!vec3_components_in_range(obj->u_obj.pl.normal, -1.0f, 1.0f))
		return (object_error(obj, line, "pl: normal out of range [-1,1]"));
	if (!vec3_is_normalized(obj->u_obj.pl.normal))
		return (object_error(obj, line, "pl: not normalized normal"));
	if (!parse_color_255(tokens[3], &obj->u_obj.pl.color))
		return (object_error(obj, line, "pl: invalid color"));
	obj->next = NULL;
	scene_add_object(scene, obj);
	return (parse_ok());
}
/*
* Purpose: Decode a plane definition, enforcing normalized normals and RGB.
* Workflow: Allocate, parse point/normal/color, and push onto the object list.
* Guarantees: Rejects malformed data and cleans up allocations on error.
*/

t_parse_result	parse_cy(char **tkns, int line, t_scene *scene)
{
	t_object	*obj;

	if (!tkns[1] || !tkns[2] || !tkns[3] || !tkns[4] || !tkns[5] || tkns[6])
		return (parse_error(line, "cy: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "cy: not enough memory"));
	obj->type = OBJ_CYLINDER;
	if (!parse_vec3(tkns[1], &obj->u_obj.cy.center))
		return (object_error(obj, line, "cy: invalid centre"));
	if (!parse_vec3(tkns[2], &obj->u_obj.cy.axis))
		return (object_error(obj, line, "cy: invalid axis"));
	if (!vec3_components_in_range(obj->u_obj.cy.axis, -1.0f, 1.0f))
		return (object_error(obj, line, "cy: axis out of range [-1,1]"));
	if (!vec3_is_normalized(obj->u_obj.cy.axis))
		return (object_error(obj, line, "cy: not normalized axis"));
	if (!parse_float(tkns[3], &obj->u_obj.cy.di) || obj->u_obj.cy.di <= 0.0f)
		return (object_error(obj, line, "cy: invalid diameter"));
	if (!parse_float(tkns[4], &obj->u_obj.cy.he) || obj->u_obj.cy.he <= 0.0f)
		return (object_error(obj, line, "cy: invalid height"));
	if (!parse_color_255(tkns[5], &obj->u_obj.cy.color))
		return (object_error(obj, line, "cy: invalid color"));
	obj->next = NULL;
	scene_add_object(scene, obj);
	return (parse_ok());
}
/*
* Purpose: Read a cylinder entry, checking axis normalization and dimensions.
* Actions: Allocate, parse center/axis/diameter/height/color, then link object.
* Failure: Returns descriptive parse_error while freeing allocated memory.
*/
