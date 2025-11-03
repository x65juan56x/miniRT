#include <math.h>
#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include_bonus/parser_internal_bonus.h"
#include "../../include_bonus/bump_bonus.h"
#include "../../include_bonus/scene_bonus.h"

static t_parse_result	tr_create_object(char **tok, int line, t_object **out)
{
	t_object	*obj;

	if (!tok[1] || !tok[2] || !tok[3] || !tok[4])
		return (parse_error(line, "tr: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "tr: not enough memory"));
	obj->type = OBJ_TRIANGLE;
	obj->next = NULL;
	*out = obj;
	return (parse_ok());
}

static t_parse_result	tr_parse_attributes(char **tok, int line, t_object *obj)
{
	if (!parse_vec3(tok[1], &obj->u_obj.tr.a))
		return (obj_error(obj, line, "tr: invalid vertex a"));
	if (!parse_vec3(tok[2], &obj->u_obj.tr.b))
		return (obj_error(obj, line, "tr: invalid vertex b"));
	if (!parse_vec3(tok[3], &obj->u_obj.tr.c))
		return (obj_error(obj, line, "tr: invalid vertex c"));
	if (!parse_color_255(tok[4], &obj->u_obj.tr.color))
		return (obj_error(obj, line, "tr: invalid color"));
	return (parse_ok());
}

static void	tr_build_basis(t_triangle *tr)
{
	tr->has_checker = 0;
	tr->checker_scale = 1.0f;
	tr->has_bump = 0;
	tr->bump_strength = 0.0f;
	tr->bump = NULL;
	tr->material = NULL;
}

static t_parse_result	tr_parse_options(char **tok, int line,
		t_object *obj, int *idx)
{
	t_bump_target	bump;

	bump.has_bump = &obj->u_obj.tr.has_bump;
	bump.map = &obj->u_obj.tr.bump;
	bump.strength = &obj->u_obj.tr.bump_strength;
	if (tok[*idx] && ft_strncmp(tok[*idx], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tok, *idx, bump))
			return (obj_error(obj, line,
					"tr: invalid bump (bm <png> <strength>)"));
		*idx += 3;
	}
	else if (tok[*idx] && ft_strncmp(tok[*idx], "cb", 3) == 0)
	{
		if (!parse_opt_checker(tok, *idx, &obj->u_obj.tr.has_checker,
				&obj->u_obj.tr.checker_scale))
			return (obj_error(obj, line,
					"tr: invalid checker (cb <scale>)"));
		*idx += 2;
	}
	return (parse_ok());
}

// Format: tr x1,y1,z1 x2,y2,z2 x3,y3,z3 r,g,b
t_parse_result	parse_tr(char **tok, int line, t_scene *scene)
{
	t_object		*obj;
	t_parse_result	result;
	int				opt_idx;
	t_parse_result	mat_res;

	obj = NULL;
	result = tr_create_object(tok, line, &obj);
	if (!result.ok)
		return (result);
	result = tr_parse_attributes(tok, line, obj);
	if (!result.ok)
		return (result);
	tr_build_basis(&obj->u_obj.tr);
	opt_idx = 5;
	result = tr_parse_options(tok, line, obj, &opt_idx);
	if (!result.ok)
		return (result);
	mat_res = parse_specular_info(tok + opt_idx, line, "tr",
			&obj->u_obj.tr.material);
	if (!mat_res.ok)
		return (free_tr_hp_with_addons(obj), mat_res);
	aux_triangle(&obj->u_obj.tr);
	return (scene_add_object(scene, obj), parse_ok());
}
/*
* Purpose: Parse a triangle primitive from three 3D points and an RGB color.
* Notes: This is an extension to support models converted by obj_to_rt.
*/
