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

// - idx: posición donde podría aparecer "cb"
// - Devuelve 1 si es válido (presente o ausente), 0 si formato inválido
// - Si está presente, exige que no haya tokens extra (fin de línea) y marca has_checker=1
static int	parse_opt_checker(char **tokens, int idx, int *has_checker, float *out_scale)
{
	*has_checker = 0;
	if (!tokens[idx])
		return (1);
	if (ft_strncmp(tokens[idx], "cb", 3) != 0)
		return (0);
	if (!tokens[idx + 1] || tokens[idx + 2]) // solo "cb <scale>" y fin
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
	if (!tokens[idx + 1] || !tokens[idx + 2] || tokens[idx + 3])
		return (0);
	if (!parse_float(tokens[idx + 2], out_strength) || *out_strength < 0.0f)
		return (0);
	*out_bump = bump_load_png(tokens[idx + 1]);
	if (!*out_bump)
		return (0);
	*has_bump = 1;
	return (1);
}

t_parse_result	parse_sp(char **tokens, int line, t_scene *scene)
{
	t_object	*obj;
	int			done;

	if (!tokens[1] || !tokens[2] || !tokens[3])
		return (parse_error(line, "sp: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "sp: not enough memory"));
	obj->type = OBJ_SPHERE;
	if (!parse_vec3(tokens[1], &obj->u_obj.sp.center))
		return (obj_error(obj, line, "sp: invalid centre"));
	if (!parse_float(tokens[2], &obj->u_obj.sp.di) || obj->u_obj.sp.di <= 0.0f)
		return (obj_error(obj, line, "sp: invalid diameter"));
	if (!parse_color_255(tokens[3], &obj->u_obj.sp.color))
		return (obj_error(obj, line, "sp: invalid color"));
	obj->u_obj.sp.has_checker = 0;
	obj->u_obj.sp.checker_scale = 1.0f; // also used as UV scale for bump
	obj->u_obj.sp.has_bump = 0;
	obj->u_obj.sp.bump_strength = 0.0f;
	obj->u_obj.sp.bump = NULL;
	if (tokens[4] && ft_strncmp(tokens[4], "bm", 3) == 0)
		done = parse_opt_bump(tokens, 4, &obj->u_obj.sp.has_bump,
				&obj->u_obj.sp.bump_strength, &obj->u_obj.sp.bump);
	else
		done = parse_opt_checker(tokens, 4, &obj->u_obj.sp.has_checker,
				&obj->u_obj.sp.checker_scale);
	if (!done)
		return (obj_error(obj, line, "sp: invalid checker (cb <scale>)"));
	aux_sphere(&obj->u_obj.sp);
	obj->next = NULL;
	scene_add_object(scene, obj);
	return (parse_ok());
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
}

t_parse_result	parse_pl(char **tok, int line, t_scene *scene)
{
	t_object		*obj;
	t_parse_result	result;

	result = pl_create_object(tok, line, &obj);
	if (!result.ok)
		return (result);
	result = pl_parse_attributes(tok, line, obj);
	if (!result.ok)
		return (result);
	plane_build_basis(&obj->u_obj.pl);
	if (tok[4] && ft_strncmp(tok[4], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tok, 4, &obj->u_obj.pl.has_bump,
				&obj->u_obj.pl.bump_strength, &obj->u_obj.pl.bump))
			return (obj_error(obj, line, "pl: invalid bump (bm <png> <strength>)"));
	}
	else if (!parse_opt_checker(tok, 4, &obj->u_obj.pl.has_checker,
			&obj->u_obj.pl.checker_scale))
		return (obj_error(obj, line, "pl: invalid checker (cb <scale>)"));
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
	if (tkns[6] && ft_strncmp(tkns[6], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tkns, 6, &obj->u_obj.cy.has_bump,
				&obj->u_obj.cy.bump_strength, &obj->u_obj.cy.bump))
			return (obj_error(obj, line, "cy: invalid bump (bm <png> <strength>)"));
	}
	else if (tkns[6])
	{
		int ok = parse_opt_checker(tkns, 6, &obj->u_obj.cy.has_checker,
				&obj->u_obj.cy.checker_scale);
		if (!ok)
			return (obj_error(obj, line, "cy: invalid checker (cb <scale>)"));
	}
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
}

t_parse_result	parse_hp(char **tok, int line, t_scene *scene)
{
	t_object		*obj;
	t_parse_result	result;
//	int				cbcons;

	result = hp_create_object(tok, line, &obj);
	if (!result.ok)
		return (result);
	result = hp_parse_attributes(tok, line, obj);
	if (!result.ok)
		return (result);
	hp_build_basis(&obj->u_obj.hp);
	if (tok[7] && ft_strncmp(tok[7], "bm", 3) == 0)
	{
		if (!parse_opt_bump(tok, 7, &obj->u_obj.hp.has_bump,
				&obj->u_obj.hp.bump_strength, &obj->u_obj.hp.bump))
			return (obj_error(obj, line,
				"hp: invalid bump (bm <png> <strength>)"));
	}
	else if (!parse_opt_checker(tok, 7, &obj->u_obj.hp.has_checker,
			&obj->u_obj.hp.checker_scale))
		return (obj_error(obj, line, "hp: invalid checker (cb <scale>)"));
	aux_hparab(&obj->u_obj.hp);
	scene_add_object(scene, obj);
	return (parse_ok());
}

// Format: tr x1,y1,z1 x2,y2,z2 x3,y3,z3 r,g,b
t_parse_result	parse_tr(char **tokens, int line, t_scene *scene)
{
	t_object	*obj;
	int			cbcons;

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
    if (tokens[5] && ft_strncmp(tokens[5], "bm", 3) == 0)
    {
	if (!parse_opt_bump(tokens, 5, &obj->u_obj.tr.has_bump,
		&obj->u_obj.tr.bump_strength, &obj->u_obj.tr.bump))
	    return (obj_error(obj, line, "tr: invalid bump (bm <png> <strength>)"));
    }
    else
    {
	cbcons = parse_opt_checker(tokens, 5, &obj->u_obj.tr.has_checker,
		&obj->u_obj.tr.checker_scale);
	if (!cbcons)
	    return (obj_error(obj, line, "tr: invalid checker (cb <scale>)"));
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
