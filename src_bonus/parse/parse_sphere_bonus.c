#include <math.h>
#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include_bonus/parser_internal_bonus.h"
#include "../../include_bonus/bump_bonus.h"
#include "../../include_bonus/scene_bonus.h"

static t_parse_result	sp_create_object(char **tok, int line, t_object **out)
{
	t_object	*obj;

	if (!tok[1] || !tok[2] || !tok[3])
		return (parse_error(line, "sp: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "sp: not enough memory"));
	obj->type = OBJ_SPHERE;
	obj->next = NULL;
	*out = obj;
	return (parse_ok());
}

static t_parse_result	sp_parse_attributes(char **tok, int line, t_object *obj)
{
	if (!parse_vec3(tok[1], &obj->u_obj.sp.center))
		return (obj_error(obj, line, "sp: invalid centre"));
	if (!parse_float(tok[2], &obj->u_obj.sp.di) || obj->u_obj.sp.di <= 0.0f)
		return (obj_error(obj, line, "sp: invalid diameter"));
	if (!parse_color_255(tok[3], &obj->u_obj.sp.color))
		return (obj_error(obj, line, "sp: invalid color"));
	return (parse_ok());
}

static void	sp_build_basis(t_sphere *sp)
{
	sp->has_checker = 0;
	sp->checker_scale = 1.0f;
	sp->has_bump = 0;
	sp->bump_strength = 0.0f;
	sp->bump = NULL;
	sp->material = NULL;
}

static t_parse_result	sp_parse_options(char **tok, int line,
		t_object *obj, int *idx)
{
	t_bump_target	bump;

	bump.has_bump = &obj->u_obj.sp.has_bump;
	bump.map = &obj->u_obj.sp.bump;
	bump.strength = &obj->u_obj.sp.bump_strength;
	if (tok[*idx] && ft_strncmp(tok[*idx], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tok, *idx, bump))
			return (obj_error(obj, line,
					"sp: invalid bump (bm <png> <strength>)"));
		*idx += 3;
	}
	else if (tok[*idx] && ft_strncmp(tok[*idx], "cb", 3) == 0)
	{
		if (!parse_opt_checker(tok, *idx, &obj->u_obj.sp.has_checker,
				&obj->u_obj.sp.checker_scale))
			return (obj_error(obj, line,
					"sp: invalid checker (cb <scale>)"));
		*idx += 2;
	}
	return (parse_ok());
}

t_parse_result	parse_sp(char **tok, int line, t_scene *scene)
{
	t_object		*obj;
	t_parse_result	result;
	int				opt_idx;
	t_parse_result	mat_res;

	obj = NULL;
	result = sp_create_object(tok, line, &obj);
	if (!result.ok)
		return (result);
	result = sp_parse_attributes(tok, line, obj);
	if (!result.ok)
		return (result);
	sp_build_basis(&obj->u_obj.sp);
	opt_idx = 4;
	result = sp_parse_options(tok, line, obj, &opt_idx);
	if (!result.ok)
		return (result);
	mat_res = parse_specular_info(tok + opt_idx, line, "sp",
			&obj->u_obj.sp.material);
	if (!mat_res.ok)
		return (free_sp_pl_cy_with_addons(obj), mat_res);
	aux_sphere(&obj->u_obj.sp);
	return (scene_add_object(scene, obj), parse_ok());
}
/*
* Purpose: Parse a sphere entry, validating format, geometry, and color.
* Steps: Allocate, decode center/diameter/color, and insert into the scene.
* Errors: Frees the object and returns a detailed message on failure.
*/
