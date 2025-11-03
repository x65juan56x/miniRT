#include <math.h>
#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include_bonus/parser_internal_bonus.h"
#include "../../include_bonus/bump_bonus.h"
#include "../../include_bonus/scene_bonus.h"

static t_parse_result	hp_create_object(char **tok, int line, t_object **out)
{
	t_object	*obj;

	if (!tok[1] || !tok[2] || !tok[3] || !tok[4] || !tok[5] || !tok[6])
		return (parse_error(line, "hp: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "hp: not enough memory"));
	obj->type = OBJ_HPARABOLOID;
	obj->next = NULL;
	*out = obj;
	return (parse_ok());
}

static t_parse_result	hp_parse_attributes(char **tok, int line, t_object *obj)
{
	if (!parse_vec3(tok[1], &obj->u_obj.hp.center))
		return (obj_error(obj, line, "hp: invalid centre"));
	if (!parse_vec3(tok[2], &obj->u_obj.hp.axis))
		return (obj_error(obj, line, "hp: invalid axis"));
	if (!vec3_components_in_range(obj->u_obj.hp.axis, -1.0f, 1.0f))
		return (obj_error(obj, line, "hp: axis out of range [-1,1]"));
	if (!vec3_is_normalized(obj->u_obj.hp.axis))
		return (obj_error(obj, line, "hp: not normalized axis"));
	if (!parse_float(tok[3], &obj->u_obj.hp.rx) || obj->u_obj.hp.rx <= 0.0f)
		return (obj_error(obj, line, "hp: invalid rx"));
	if (!parse_float(tok[4], &obj->u_obj.hp.ry) || obj->u_obj.hp.ry <= 0.0f)
		return (obj_error(obj, line, "hp: invalid ry"));
	if (!parse_float(tok[5], &obj->u_obj.hp.height)
		|| obj->u_obj.hp.height <= 0.0f)
		return (obj_error(obj, line, "hp: invalid height"));
	if (!parse_color_255(tok[6], &obj->u_obj.hp.color))
		return (obj_error(obj, line, "hp: invalid color"));
	return (parse_ok());
}

static void	hp_build_basis(t_hparab *hp)
{
	hp->has_checker = 0;
	hp->checker_scale = 1.0f;
	hp->has_bump = 0;
	hp->bump_strength = 0.0f;
	hp->bump = NULL;
	hp->material = NULL;
}
// checker_scale is also used as UV scale for bump

static t_parse_result	hp_parse_options(char **tok, int line,
		t_object *obj, int *idx)
{
	t_bump_target	bump;

	bump.has_bump = &obj->u_obj.hp.has_bump;
	bump.map = &obj->u_obj.hp.bump;
	bump.strength = &obj->u_obj.hp.bump_strength;
	if (tok[*idx] && ft_strncmp(tok[*idx], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tok, *idx, bump))
			return (obj_error(obj, line,
					"hp: invalid bump (bm <png> <strength>)"));
		*idx += 3;
	}
	else if (tok[*idx] && ft_strncmp(tok[*idx], "cb", 3) == 0)
	{
		if (!parse_opt_checker(tok, *idx, &obj->u_obj.hp.has_checker,
				&obj->u_obj.hp.checker_scale))
			return (obj_error(obj, line, "hp: invalid checker (cb <scale>)"));
		*idx += 2;
	}
	return (parse_ok());
}

t_parse_result	parse_hp(char **tok, int line, t_scene *scene)
{
	t_object		*obj;
	t_parse_result	result;
	int				opt_idx;
	t_parse_result	mat_res;

	obj = NULL;
	result = hp_create_object(tok, line, &obj);
	if (!result.ok)
		return (result);
	result = hp_parse_attributes(tok, line, obj);
	if (!result.ok)
		return (result);
	hp_build_basis(&obj->u_obj.hp);
	opt_idx = 7;
	result = hp_parse_options(tok, line, obj, &opt_idx);
	if (!result.ok)
		return (result);
	mat_res = parse_specular_info(tok + opt_idx, line, "hp",
			&obj->u_obj.hp.material);
	if (!mat_res.ok)
		return (free_tr_hp_with_addons(obj), mat_res);
	aux_hparab(&obj->u_obj.hp);
	return (scene_add_object(scene, obj), parse_ok());
}
