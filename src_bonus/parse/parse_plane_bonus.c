#include <math.h>
#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include_bonus/parser_internal_bonus.h"
#include "../../include_bonus/bump_bonus.h"
#include "../../include_bonus/scene_bonus.h"

static t_parse_result	pl_create_object(char **tok, int line, t_object **out)
{
	t_object	*obj;

	if (!tok[1] || !tok[2] || !tok[3])
		return (parse_error(line, "pl: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "pl: not enough memory"));
	obj->type = OBJ_PLANE;
	obj->next = NULL;
	*out = obj;
	return (parse_ok());
}

static t_parse_result	pl_parse_attributes(char **tok, int line, t_object *obj)
{
	if (!parse_vec3(tok[1], &obj->u_obj.pl.point))
		return (obj_error(obj, line, "pl: invalid point"));
	if (!parse_vec3(tok[2], &obj->u_obj.pl.normal))
		return (obj_error(obj, line, "pl: invalid normal"));
	if (!vec3_components_in_range(obj->u_obj.pl.normal, -1.0f, 1.0f))
		return (obj_error(obj, line, "pl: normal out of range [-1,1]"));
	if (!vec3_is_normalized(obj->u_obj.pl.normal))
		return (obj_error(obj, line, "pl: not normalized normal"));
	if (!parse_color_255(tok[3], &obj->u_obj.pl.color))
		return (obj_error(obj, line, "pl: invalid color"));
	return (parse_ok());
}

static void	plane_build_basis(t_plane *pl)
{
	pl->has_checker = 0;
	pl->checker_scale = 1.0f;
	pl->has_bump = 0;
	pl->bump_strength = 0.0f;
	pl->bump = NULL;
	pl->material = NULL;
}

static t_parse_result	pl_parse_options(char **tok, int line,
		t_object *obj, int *idx)
{
	t_bump_target	bump;

	bump.has_bump = &obj->u_obj.pl.has_bump;
	bump.map = &obj->u_obj.pl.bump;
	bump.strength = &obj->u_obj.pl.bump_strength;
	if (tok[*idx] && ft_strncmp(tok[*idx], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tok, *idx, bump))
			return (obj_error(obj, line,
					"pl: invalid bump (bm <png> <strength>)"));
		*idx += 3;
	}
	else if (tok[*idx] && ft_strncmp(tok[*idx], "cb", 3) == 0)
	{
		if (!parse_opt_checker(tok, *idx, &obj->u_obj.pl.has_checker,
				&obj->u_obj.pl.checker_scale))
			return (obj_error(obj, line,
					"pl: invalid checker (cb <scale>)"));
		*idx += 2;
	}
	return (parse_ok());
}

t_parse_result	parse_pl(char **tok, int line, t_scene *scene)
{
	t_object		*obj;
	t_parse_result	result;
	int				opt_idx;
	t_parse_result	mat_res;

	obj = NULL;
	result = pl_create_object(tok, line, &obj);
	if (!result.ok)
		return (result);
	result = pl_parse_attributes(tok, line, obj);
	if (!result.ok)
		return (result);
	plane_build_basis(&obj->u_obj.pl);
	opt_idx = 4;
	result = pl_parse_options(tok, line, obj, &opt_idx);
	if (!result.ok)
		return (result);
	mat_res = parse_specular_info(tok + opt_idx, line, "pl",
			&obj->u_obj.pl.material);
	if (!mat_res.ok)
		return (free_sp_pl_cy_with_addons(obj), mat_res);
	aux_plane(&obj->u_obj.pl);
	return (scene_add_object(scene, obj), parse_ok());
}
/*
* Purpose: Decode a plane definition, enforcing normalized normals and RGB.
* Workflow: Allocate, parse point/normal/color, and push onto the object list.
* Guarantees: Rejects malformed data and cleans up allocations on error.
*/
