#include <math.h>
#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include_bonus/parser_internal_bonus.h"
#include "../../include_bonus/bump_bonus.h"
#include "../../include_bonus/scene_bonus.h"

static t_parse_result	obj_error(t_object *obj, int line, const char *msg)
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

static void	free_sp_pl_cy_with_addons(t_object *obj)
{
	if (!obj)
		return ;
	if (obj->type == OBJ_SPHERE)
	{
		if (obj->u_obj.sp.bump)
			bump_free(obj->u_obj.sp.bump);
		if (obj->u_obj.sp.material)
			free(obj->u_obj.sp.material);
	}
	else if (obj->type == OBJ_PLANE)
	{
		if (obj->u_obj.pl.bump)
			bump_free(obj->u_obj.pl.bump);
		if (obj->u_obj.pl.material)
			free(obj->u_obj.pl.material);
	}
	else if (obj->type == OBJ_CYLINDER)
	{
		if (obj->u_obj.cy.bump)
			bump_free(obj->u_obj.cy.bump);
		if (obj->u_obj.cy.material)
			free(obj->u_obj.cy.material);
	}
	free(obj);
}

static void	free_tr_hp_with_addons(t_object *obj)
{
	if (!obj)
		return ;
	if (obj->type == OBJ_TRIANGLE)
	{
		if (obj->u_obj.tr.bump)
			bump_free(obj->u_obj.tr.bump);
		if (obj->u_obj.tr.material)
			free(obj->u_obj.tr.material);
	}
	else if (obj->type == OBJ_HPARABOLOID)
	{
		if (obj->u_obj.hp.bump)
			bump_free(obj->u_obj.hp.bump);
		if (obj->u_obj.hp.material)
			free(obj->u_obj.hp.material);
	}
	free(obj);
}

static t_parse_result	spec_error(int line, const char *tag, const char *msg)
{
	char	*prefix;
	char	*full;
	t_parse_result	res;

	if (!tag || !*tag)
		return (parse_error(line, msg));
	prefix = ft_strjoin(tag, ": ");
	if (!prefix)
		return (parse_error(line, msg));
	full = ft_strjoin(prefix, msg);
	free(prefix);
	if (!full)
		return (parse_error(line, msg));
	res = parse_error(line, full);
	free(full);
	return (res);
}

// - idx: posición donde podría aparecer "cb"
// - Devuelve 1 si es válido (presente o ausente), 0 si formato inválido
// - Si está presente valida "cb <scale>" y deja que el llamador avance el índice
static int	parse_opt_checker(char **tokens, int idx, int *has_checker, float *out_scale)
{
	*has_checker = 0;
	if (!tokens[idx])
		return (1);
	if (ft_strncmp(tokens[idx], "cb", 3) != 0)
		return (0);
	if (!tokens[idx + 1])
		return (0);
	if (!parse_float(tokens[idx + 1], out_scale) || *out_scale <= 0.0f)
		return (0);
	*has_checker = 1;
	return (1);
}

static int parse_opt_bump(char **tokens, int idx, int *has_bump,
		float *out_strength, t_bumpmap **out_bump)
{
	*has_bump = 0;
	*out_bump = NULL;
	if (!tokens[idx])
		return (1);
	if (ft_strncmp(tokens[idx], "bm", 3) != 0)
		return (1);
	if (!tokens[idx + 1] || !tokens[idx + 2])
		return (0);
	if (!parse_float(tokens[idx + 2], out_strength) || *out_strength < 0.0f)
		return (0);
	*out_bump = bump_load_png(tokens[idx + 1]);
	if (!*out_bump)
		return (0);
	*has_bump = 1;
	return (1);
}

static t_parse_result	parse_specular_info(char **tokens, int line,
		const char *tag, t_material **out_mat)
{
	t_material	*mat;
	float		ks;
	float		shininess;
	float		reflectivity;
	int			idx;
	int			has_any;
	int			has_ks;
	t_spec_model	model;

	*out_mat = NULL;
	if (!tokens || !tokens[0])
		return (parse_ok());
	ks = 0.0f;
	shininess = 0.0f;
	reflectivity = 0.0f;
	idx = 0;
	has_any = 0;
	has_ks = 0;
	model = SPEC_MODEL_BLINN;
	if (ft_strncmp(tokens[idx], "ks", 3) == 0)
	{
		has_any = 1;
		has_ks = 1;
		if (!tokens[idx + 1] || !tokens[idx + 2])
			return (spec_error(line, tag,
				"material expects 'ks <ratio> <shininess>'"));
		if (!parse_float(tokens[idx + 1], &ks) || ks < 0.0f || ks > 1.0f)
			return (spec_error(line, tag, "invalid ks (use 0 <= ks <= 1)"));
		if (!parse_float(tokens[idx + 2], &shininess) || shininess <= 0.0f)
			return (spec_error(line, tag, "invalid shininess (> 0 required)"));
		idx += 3;
		if (tokens[idx] && ft_strncmp(tokens[idx], "phong", 6) == 0)
		{
			model = SPEC_MODEL_PHONG;
			idx++;
		}
		else if (tokens[idx]
			&& (ft_strncmp(tokens[idx], "blinn", 6) == 0
				|| ft_strncmp(tokens[idx], "blinn-phong", 12) == 0))
		{
			model = SPEC_MODEL_BLINN;
			idx++;
		}
	}
	else if (ft_strncmp(tokens[idx], "kr", 3) != 0)
		return (spec_error(line, tag, "unknown material token (ks|kr)"));
	if (tokens[idx])
	{
		if (ft_strncmp(tokens[idx], "kr", 3) != 0)
		{
			if (has_ks)
				return (spec_error(line, tag,
					"unexpected tokens after material definition"));
			return (spec_error(line, tag, "unknown material token (ks|kr)"));
		}
		if (!tokens[idx + 1])
			return (spec_error(line, tag, "kr expects '<ratio>'"));
		if (!parse_float(tokens[idx + 1], &reflectivity)
			|| reflectivity < 0.0f || reflectivity > 1.0f)
			return (spec_error(line, tag,
				"invalid kr (use 0 <= kr <= 1)"));
		has_any = 1;
		idx += 2;
	}
	if (tokens[idx])
		return (spec_error(line, tag,
			"unexpected tokens after material definition"));
	if (!has_any)
		return (parse_ok());
	mat = (t_material *)malloc(sizeof(*mat));
	if (!mat)
		return (spec_error(line, tag, "not enough memory for material"));
	mat->albedo = v3(0.0f, 0.0f, 0.0f);
	mat->ks = ks;
	mat->shininess = shininess;
	mat->model = model;
	mat->reflectivity = reflectivity;
	*out_mat = mat;
	return (parse_ok());
}

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
	sp->checker_scale = 1.0f; // also used as UV scale for bump
	sp->has_bump = 0;
	sp->bump_strength = 0.0f;
	sp->bump = NULL;
	sp->material = NULL;
}

static t_parse_result	sp_parse_options(char **tok, int line,
		t_object *obj, int *idx)
{
	if (tok[*idx] && ft_strncmp(tok[*idx], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tok, *idx, &obj->u_obj.sp.has_bump,
				&obj->u_obj.sp.bump_strength, &obj->u_obj.sp.bump))
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
	obj->next = NULL;
	return (scene_add_object(scene, obj), parse_ok());
}
/*
* Purpose: Parse a sphere entry, validating format, geometry, and color.
* Steps: Allocate, decode center/diameter/color, and insert into the scene.
* Errors: Frees the object and returns a detailed message on failure.
*/

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
	if (tok[opt_idx] && ft_strncmp(tok[opt_idx], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tok, opt_idx, &obj->u_obj.pl.has_bump,
				&obj->u_obj.pl.bump_strength, &obj->u_obj.pl.bump))
			return (obj_error(obj, line,
				"pl: invalid bump (bm <png> <strength>)"));
		opt_idx += 3;
	}
	else if (tok[opt_idx] && ft_strncmp(tok[opt_idx], "cb", 3) == 0)
	{
		if (!parse_opt_checker(tok, opt_idx, &obj->u_obj.pl.has_checker,
				&obj->u_obj.pl.checker_scale))
			return (obj_error(obj, line,
				"pl: invalid checker (cb <scale>)"));
		opt_idx += 2;
	}
	mat_res = parse_specular_info(tok + opt_idx, line, "pl",
			&obj->u_obj.pl.material);
	if (!mat_res.ok)
		return (free_sp_pl_cy_with_addons(obj), mat_res);
	aux_plane(&obj->u_obj.pl);
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
	int		opt_idx;
	t_parse_result	mat_res;

	if (!tkns[1] || !tkns[2] || !tkns[3] || !tkns[4] || !tkns[5])
		return (parse_error(line, "cy: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "cy: not enough memory"));
	obj->type = OBJ_CYLINDER;
	if (!parse_vec3(tkns[1], &obj->u_obj.cy.center))
		return (obj_error(obj, line, "cy: invalid centre"));
	if (!parse_vec3(tkns[2], &obj->u_obj.cy.axis))
		return (obj_error(obj, line, "cy: invalid axis"));
	if (!vec3_components_in_range(obj->u_obj.cy.axis, -1.0f, 1.0f))
		return (obj_error(obj, line, "cy: axis out of range [-1,1]"));
	if (!vec3_is_normalized(obj->u_obj.cy.axis))
		return (obj_error(obj, line, "cy: not normalized axis"));
	if (!parse_float(tkns[3], &obj->u_obj.cy.di) || obj->u_obj.cy.di <= 0.0f)
		return (obj_error(obj, line, "cy: invalid diameter"));
	if (!parse_float(tkns[4], &obj->u_obj.cy.he) || obj->u_obj.cy.he <= 0.0f)
		return (obj_error(obj, line, "cy: invalid height"));
	if (!parse_color_255(tkns[5], &obj->u_obj.cy.color))
		return (obj_error(obj, line, "cy: invalid color"));
		// Bonus options: either bump (bm <png> <strength>) or checker (cb <scale>)
	obj->u_obj.cy.has_checker = 0;
	obj->u_obj.cy.checker_scale = 1.0f;
	obj->u_obj.cy.has_bump = 0;
	obj->u_obj.cy.bump_strength = 0.0f;
	obj->u_obj.cy.bump = NULL;
	obj->u_obj.cy.material = NULL;
	opt_idx = 6;
	if (tkns[opt_idx] && ft_strncmp(tkns[opt_idx], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tkns, opt_idx, &obj->u_obj.cy.has_bump,
				&obj->u_obj.cy.bump_strength, &obj->u_obj.cy.bump))
			return (obj_error(obj, line, "cy: invalid bump (bm <png> <strength>)"));
		opt_idx += 3;
	}
	else if (tkns[opt_idx] && ft_strncmp(tkns[opt_idx], "cb", 3) == 0)
	{
		if (!parse_opt_checker(tkns, opt_idx, &obj->u_obj.cy.has_checker,
				&obj->u_obj.cy.checker_scale))
			return (obj_error(obj, line, "cy: invalid checker (cb <scale>)"));
		opt_idx += 2;
	}
	mat_res = parse_specular_info(tkns + opt_idx, line, "cy",
			&obj->u_obj.cy.material);
	if (!mat_res.ok)
		return (free_sp_pl_cy_with_addons(obj), mat_res);
	aux_cylinder(&obj->u_obj.cy);
	obj->next = NULL;
	scene_add_object(scene, obj);
	return (parse_ok());
}
/*
* Purpose: Read a cylinder entry, checking axis normalization and dimensions.
* Actions: Allocate, parse center/axis/diameter/height/color, then link object.
* Failure: Returns descriptive parse_error while freeing allocated memory.
*/

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

static void hp_build_basis(t_hparab *hp)
{
	hp->has_checker = 0;
	hp->checker_scale = 1.0f; // also used as UV scale for bump
	hp->has_bump = 0;
	hp->bump_strength = 0.0f;
	hp->bump = NULL;
	hp->material = NULL;
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
	if (tok[opt_idx] && ft_strncmp(tok[opt_idx], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tok, opt_idx, &obj->u_obj.hp.has_bump,
				&obj->u_obj.hp.bump_strength, &obj->u_obj.hp.bump))
			return (obj_error(obj, line,
				"hp: invalid bump (bm <png> <strength>)"));
		opt_idx += 3;
	}
	else if (tok[opt_idx] && ft_strncmp(tok[opt_idx], "cb", 3) == 0)
	{
		if (!parse_opt_checker(tok, opt_idx, &obj->u_obj.hp.has_checker,
				&obj->u_obj.hp.checker_scale))
			return (obj_error(obj, line, "hp: invalid checker (cb <scale>)"));
		opt_idx += 2;
	}
	mat_res = parse_specular_info(tok + opt_idx, line, "hp",
			&obj->u_obj.hp.material);
	if (!mat_res.ok)
		return (free_tr_hp_with_addons(obj), mat_res);
	aux_hparab(&obj->u_obj.hp);
	scene_add_object(scene, obj);
	return (parse_ok());
}

// Format: tr x1,y1,z1 x2,y2,z2 x3,y3,z3 r,g,b
t_parse_result	parse_tr(char **tokens, int line, t_scene *scene)
{
	t_object	*obj;
	int		opt_idx;
	t_parse_result	mat_res;

	if (!tokens[1] || !tokens[2] || !tokens[3] || !tokens[4])
		return (parse_error(line, "tr: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "tr: not enough memory"));
	obj->type = OBJ_TRIANGLE;
	if (!parse_vec3(tokens[1], &obj->u_obj.tr.a))
		return (obj_error(obj, line, "tr: invalid vertex a"));
	if (!parse_vec3(tokens[2], &obj->u_obj.tr.b))
		return (obj_error(obj, line, "tr: invalid vertex b"));
	if (!parse_vec3(tokens[3], &obj->u_obj.tr.c))
		return (obj_error(obj, line, "tr: invalid vertex c"));
	if (!parse_color_255(tokens[4], &obj->u_obj.tr.color))
		return (obj_error(obj, line, "tr: invalid color"));
	obj->u_obj.tr.has_checker = 0;
	obj->u_obj.tr.checker_scale = 1.0f;
	obj->u_obj.tr.has_bump = 0;
	obj->u_obj.tr.bump_strength = 0.0f;
	obj->u_obj.tr.bump = NULL;
	obj->u_obj.tr.material = NULL;
	opt_idx = 5;
	if (tokens[opt_idx] && ft_strncmp(tokens[opt_idx], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tokens, opt_idx, &obj->u_obj.tr.has_bump,
				&obj->u_obj.tr.bump_strength, &obj->u_obj.tr.bump))
			return (obj_error(obj, line,
				"tr: invalid bump (bm <png> <strength>)"));
		opt_idx += 3;
	}
	else if (tokens[opt_idx] && ft_strncmp(tokens[opt_idx], "cb", 3) == 0)
	{
		if (!parse_opt_checker(tokens, opt_idx, &obj->u_obj.tr.has_checker,
				&obj->u_obj.tr.checker_scale))
			return (obj_error(obj, line,
				"tr: invalid checker (cb <scale>)"));
		opt_idx += 2;
	}
	mat_res = parse_specular_info(tokens + opt_idx, line, "tr",
			&obj->u_obj.tr.material);
	if (!mat_res.ok)
	{
		free_tr_hp_with_addons(obj);
		return (mat_res);
	}
	aux_triangle(&obj->u_obj.tr);
	obj->next = NULL;
	scene_add_object(scene, obj);
	return (parse_ok());
}
/*
* Purpose: Parse a triangle primitive from three 3D points and an RGB color.
* Notes: This is an extension to support models converted by obj_to_rt.
*/
