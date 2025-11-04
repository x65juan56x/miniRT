#include "../../include_bonus/parser_internal_bonus.h"

static t_parse_result	l_parse_attributes(char **tok, int line,
		t_light_prop *l_vars)
{
	if (!parse_vec3(tok[1], &l_vars->pos))
		return (parse_error(line, "L: invalid position"));
	if (!parse_float(tok[2], &l_vars->bright))
		return (parse_error(line, "L: invalid bright"));
	if (l_vars->bright < 0.0f || l_vars->bright > 1.0f)
		return (parse_error(line, "L: bright out of range [0,1]"));
	l_vars->color = v3(1.0f, 1.0f, 1.0f);
	if (tok[3] && !parse_color_255(tok[3], &l_vars->color))
		return (parse_error(line, "L: invalid color"));
	return (parse_ok());
}
// Parse and validate light attributes: position, brightness [0,1], and optional color.

static void	l_build_basis(t_light *new_light, t_light_prop *l_vars)
{
	new_light->present = true;
	new_light->next = NULL;
	new_light->color = l_vars->color;
	new_light->bright = l_vars->bright;
	new_light->pos = l_vars->pos;
}
// Initialize a new light structure with parsed values.

t_parse_result	parse_l(char **tok, int line, t_scene *scene)
{
	t_light			*new_light;
	t_light			*last_light;
	t_parse_result	result;
	t_light_prop	l_vars;

	if (!tok[1] || !tok[2] || tok[4])
		return (parse_error(line, "L: invalid format"));
	result = l_parse_attributes(tok, line, &l_vars);
	if (!result.ok)
		return (result);
	new_light = malloc(sizeof(t_light));
	if (!new_light)
		return (parse_error(line, "L: malloc failed"));
	l_build_basis(new_light, &l_vars);
	if (!scene->light)
		scene->light = new_light;
	else
	{
		last_light = scene->light;
		while (last_light->next)
			last_light = last_light->next;
		last_light->next = new_light;
	}
	return (parse_ok());
}
/*
* Purpose: Parse a light source definition from the scene file.
* Inputs: tok (tokenized line: "L" position brightness [color]), line number, scene.
* Algorithm:
*   - Verify correct format: "L" requires exactly 2 mandatory params (position, brightness)
*     and 1 optional param (color)
*   - Parse position as a 3D coordinate (x,y,z)
*   - Parse brightness as a float in range [0,1]
*   - Parse optional color (defaults to white if not provided)
*   - Allocate memory for a new light structure
*   - Initialize the light with parsed values
*   - Add the light to the scene's linked list:
*       • If this is the first light, set it as the head
*       • Otherwise, traverse to the end and append
* Returns: parse_ok() on success, parse_error() with message on failure.
* Use: Called by the parser when it encounters an "L" line in the scene file.
*/
