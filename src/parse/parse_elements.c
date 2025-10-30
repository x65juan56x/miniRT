#include "../../include/parser_internal.h"

static int	invalid_format(char **tokens, int count)
{
	int	index;

	index = 0;
	while (tokens[index] && index < count)
		index++;
	if (tokens[index] != NULL)
		return (1);
	return (0);
}
/*
* Purpose: Verify that a token list has exactly `count` meaningful fields.
* Behavior: Returns 1 when extra tokens remain; 0 when the format is correct.
* Use: Shared guard for A/C entries where trailing data is forbidden.
*/

t_parse_result	parse_a(char **tokens, int line, t_scene *scene)
{
	if (!tokens[1] || !tokens[2])
		return (parse_error(line, "A: invalid format"));
	if (invalid_format(tokens, 3))
		return (parse_error(line, "A: invalid format"));
	if (scene->ambient.present)
		return (parse_error(line, "Duplicated A"));
	scene->ambient.present = true;
	if (!parse_float(tokens[1], &scene->ambient.ratio))
		return (parse_error(line, "A: invalid ratio"));
	if (scene->ambient.ratio < 0.0f || scene->ambient.ratio > 1.0f)
		return (parse_error(line, "A: ratio out of range [0,1]"));
	if (!parse_color_255(tokens[2], &scene->ambient.color))
		return (parse_error(line, "A: invalid color"));
	return (parse_ok());
}
/*
* Purpose: Parse the ambient light directive and populate the scene struct.
* Validates: Format, uniqueness, ratio in [0,1], and RGB components.
* Errors: Returns contextual parse_error messages for each failure mode.
*/

t_parse_result	parse_c(char **tokens, int line, t_scene *scene)
{
	if (!tokens[1] || !tokens[2] || !tokens[3])
		return (parse_error(line, "C: invalid format"));
	if (invalid_format(tokens, 4))
		return (parse_error(line, "C: invalid format"));
	if (scene->camera.present)
		return (parse_error(line, "Duplicated C"));
	scene->camera.present = true;
	if (!parse_vec3(tokens[1], &scene->camera.pos))
		return (parse_error(line, "C: invalid position"));
	if (!parse_vec3(tokens[2], &scene->camera.dir))
		return (parse_error(line, "C: invalid direction"));
	if (!vec3_components_in_range(scene->camera.dir, -1.0f, 1.0f))
		return (parse_error(line, "C: direction out of range [-1,1]"));
	if (!vec3_is_normalized(scene->camera.dir))
		return (parse_error(line, "C: not normalized direction"));
	if (!parse_float(tokens[3], &scene->camera.fov_deg))
		return (parse_error(line, "C: invalid FOV"));
	if (scene->camera.fov_deg <= 0.0f || scene->camera.fov_deg >= 180.0f)
		return (parse_error(line, "C: FOV out of range (0,180)"));
	scene->camera.dir = v3_norm(scene->camera.dir);
	return (parse_ok());
}
/*
* Purpose: Decode the camera entry, including position, direction, and FOV.
* Checks: Single declaration, vector ranges, normalization, and FOV bounds.
* Outcome: Populates `scene->camera` or returns a descriptive failure.
*/

t_parse_result	parse_l(char **tokens, int line, t_scene *scene)
{
	if (!tokens[1] || !tokens[2])
		return (parse_error(line, "L: invalid format"));
	if (tokens[4])
		return (parse_error(line, "L: invalid format"));
	if (scene->light.present)
		return (parse_error(line, "Duplicated L"));
	scene->light.present = true;
	scene->light.color = v3(1.0f, 1.0f, 1.0f);
	if (!parse_vec3(tokens[1], &scene->light.pos))
		return (parse_error(line, "L: invalid position"));
	if (!parse_float(tokens[2], &scene->light.bright))
		return (parse_error(line, "L: invalid bright"));
	if (scene->light.bright < 0.0f || scene->light.bright > 1.0f)
		return (parse_error(line, "L: bright out of range [0,1]"));
	if (tokens[3] && !parse_color_255(tokens[3], &scene->light.color))
		return (parse_error(line, "L: invalid color"));
	return (parse_ok());
}
/*
* Purpose: Interpret the point light definition, with optional color override.
* Checks: Unique declaration, valid position, brightness, and optional RGB.
* Result: Updates the scene light or reports the precise parsing failure.
*/
