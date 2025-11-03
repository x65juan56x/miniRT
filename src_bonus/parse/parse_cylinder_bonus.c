#include <math.h>
#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include_bonus/parser_internal_bonus.h"
#include "../../include_bonus/bump_bonus.h"
#include "../../include_bonus/scene_bonus.h"

static t_parse_result	cy_create_object(char **tok, int line, t_object **out)
{
	t_object	*obj;

	if (!tok[1] || !tok[2] || !tok[3] || !tok[4] || !tok[5])
		return (parse_error(line, "cy: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "cy: not enough memory"));
	obj->type = OBJ_CYLINDER;
	obj->next = NULL;
	*out = obj;
	return (parse_ok());
}

static t_parse_result	cy_parse_attributes(char **tok, int line, t_object *obj)
{
	if (!parse_vec3(tok[1], &obj->u_obj.cy.center))
		return (obj_error(obj, line, "cy: invalid centre"));
	if (!parse_vec3(tok[2], &obj->u_obj.cy.axis))
		return (obj_error(obj, line, "cy: invalid axis"));
	if (!vec3_components_in_range(obj->u_obj.cy.axis, -1.0f, 1.0f))
		return (obj_error(obj, line, "cy: axis out of range [-1,1]"));
	if (!vec3_is_normalized(obj->u_obj.cy.axis))
		return (obj_error(obj, line, "cy: not normalized axis"));
	if (!parse_float(tok[3], &obj->u_obj.cy.di) || obj->u_obj.cy.di <= 0.0f)
		return (obj_error(obj, line, "cy: invalid diameter"));
	if (!parse_float(tok[4], &obj->u_obj.cy.he) || obj->u_obj.cy.he <= 0.0f)
		return (obj_error(obj, line, "cy: invalid height"));
	if (!parse_color_255(tok[5], &obj->u_obj.cy.color))
		return (obj_error(obj, line, "cy: invalid color"));
	return (parse_ok());
}

static void	cy_build_basis(t_cyl *cy)
{
	cy->has_checker = 0;
	cy->checker_scale = 1.0f;
	cy->has_bump = 0;
	cy->bump_strength = 0.0f;
	cy->bump = NULL;
	cy->material = NULL;
}

static t_parse_result	cy_parse_options(char **tok, int line,
		t_object *obj, int *idx)
{
	t_bump_target	bump;

	bump.has_bump = &obj->u_obj.cy.has_bump;
	bump.map = &obj->u_obj.cy.bump;
	bump.strength = &obj->u_obj.cy.bump_strength;
	if (tok[*idx] && ft_strncmp(tok[*idx], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tok, *idx, bump))
			return (obj_error(obj, line,
					"cy: invalid bump (bm <png> <strength>)"));
		*idx += 3;
	}
	else if (tok[*idx] && ft_strncmp(tok[*idx], "cb", 3) == 0)
	{
		if (!parse_opt_checker(tok, *idx, &obj->u_obj.cy.has_checker,
				&obj->u_obj.cy.checker_scale))
			return (obj_error(obj, line, "cy: invalid checker (cb <scale>)"));
		*idx += 2;
	}
	return (parse_ok());
}

t_parse_result	parse_cy(char **tok, int line, t_scene *scene)
{
	t_object		*obj;
	t_parse_result	result;
	int				opt_idx;
	t_parse_result	mat_res;

	obj = NULL;
	result = cy_create_object(tok, line, &obj);
	if (!result.ok)
		return (result);
	result = cy_parse_attributes(tok, line, obj);
	if (!result.ok)
		return (result);
	cy_build_basis(&obj->u_obj.cy);
	opt_idx = 6;
	result = cy_parse_options(tok, line, obj, &opt_idx);
	if (!result.ok)
		return (result);
	mat_res = parse_specular_info(tok + opt_idx, line, "cy",
			&obj->u_obj.cy.material);
	if (!mat_res.ok)
		return (free_sp_pl_cy_with_addons(obj), mat_res);
	aux_cylinder(&obj->u_obj.cy);
	return (scene_add_object(scene, obj), parse_ok());
}
/*
* Purpose: Read a cylinder entry, checking axis normalization and dimensions.
* Actions: Allocate, parse center/axis/diameter/height/color, then link object.
* Failure: Returns descriptive parse_error while freeing allocated memory.
*/
