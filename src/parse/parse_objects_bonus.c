#include <math.h>
#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include/parser_internal_bonus.h"
#include "../../include/bump_bonus.h"

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

// - idx: posición donde podría aparecer "cb"
// - Devuelve 1 si es válido (presente o ausente), 0 si formato inválido
// - Si está presente, exige que no haya tokens extra (fin de línea) y marca has_checker=1
static int	parse_optional_checker(char **tokens, int idx, int *has_checker, float *out_scale)
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

static int parse_optional_bump(char **tokens, int idx, int *has_bump,
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
		return (object_error(obj, line, "sp: invalid centre"));
	if (!parse_float(tokens[2], &obj->u_obj.sp.di) || obj->u_obj.sp.di <= 0.0f)
		return (object_error(obj, line, "sp: invalid diameter"));
	if (!parse_color_255(tokens[3], &obj->u_obj.sp.color))
		return (object_error(obj, line, "sp: invalid color"));
	obj->u_obj.sp.has_checker = 0;
	obj->u_obj.sp.checker_scale = 1.0f; // also used as UV scale for bump
	obj->u_obj.sp.has_bump = 0;
	obj->u_obj.sp.bump_strength = 0.0f;
	obj->u_obj.sp.bump = NULL;
	if (tokens[4] && ft_strncmp(tokens[4], "bm", 3) == 0)
		done = parse_optional_bump(tokens, 4, &obj->u_obj.sp.has_bump,
				&obj->u_obj.sp.bump_strength, &obj->u_obj.sp.bump);
	else
		done = parse_optional_checker(tokens, 4, &obj->u_obj.sp.has_checker,
				&obj->u_obj.sp.checker_scale);
	if (!done)
		return (object_error(obj, line, "sp: invalid checker (cb <scale>)"));
	obj->next = NULL;
	scene_add_object(scene, obj);
	return (parse_ok());
}
/*
* Purpose: Parse a sphere entry, validating format, geometry, and color.
* Steps: Allocate, decode center/diameter/color, and insert into the scene.
* Errors: Frees the object and returns a detailed message on failure.
*/

static void	plane_build_basis(t_plane *pl)
{
	t_vec3	up;

	pl->has_checker = 0;
	pl->checker_scale = 1.0f;
	up = v3(0.0f, 1.0f, 0.0f);
	if (fabsf(v3_dot(pl->normal, up)) > 0.999f)
		up = v3(1.0f, 0.0f, 0.0f);
	pl->u = v3_norm(v3_cross(up, pl->normal));
	pl->v = v3_cross(pl->normal, pl->u);
}

// Nota: usamos la función unificada para todas las figuras

t_parse_result	parse_pl(char **tokens, int line, t_scene *scene)
{
	t_object	*obj;

	if (!tokens[1] || !tokens[2] || !tokens[3])
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
	plane_build_basis(&obj->u_obj.pl);
	obj->u_obj.pl.has_bump = 0;
	obj->u_obj.pl.bump_strength = 0.0f;
	obj->u_obj.pl.bump = NULL;
	if (tokens[4] && ft_strncmp(tokens[4], "bm", 3) == 0)
	{
		if (!parse_optional_bump(tokens, 4, &obj->u_obj.pl.has_bump,
				&obj->u_obj.pl.bump_strength, &obj->u_obj.pl.bump))
			return (object_error(obj, line, "pl: invalid bump (bm <png> <strength>)"));
	}
	else if (!parse_optional_checker(tokens, 4, &obj->u_obj.pl.has_checker,
			&obj->u_obj.pl.checker_scale))
		return (object_error(obj, line, "pl: invalid checker (cb <scale>)"));
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
		return (object_error(obj, line, "hp: invalid centre"));
	if (!parse_vec3(tok[2], &obj->u_obj.hp.axis))
		return (object_error(obj, line, "hp: invalid axis"));
	if (!vec3_components_in_range(obj->u_obj.hp.axis, -1.0f, 1.0f))
		return (object_error(obj, line, "hp: axis out of range [-1,1]"));
	if (!vec3_is_normalized(obj->u_obj.hp.axis))
		return (object_error(obj, line, "hp: not normalized axis"));
	if (!parse_float(tok[3], &obj->u_obj.hp.rx) || obj->u_obj.hp.rx <= 0.0f)
		return (object_error(obj, line, "hp: invalid rx"));
	if (!parse_float(tok[4], &obj->u_obj.hp.ry) || obj->u_obj.hp.ry <= 0.0f)
		return (object_error(obj, line, "hp: invalid ry"));
	if (!parse_float(tok[5], &obj->u_obj.hp.height)
		|| obj->u_obj.hp.height <= 0.0f)
		return (object_error(obj, line, "hp: invalid height"));
	if (!parse_color_255(tok[6], &obj->u_obj.hp.color))
		return (object_error(obj, line, "hp: invalid color"));
	return (parse_ok());
}

static void	hp_finalize(t_hparab *hp)
{
	t_vec3	up;

	up = v3(0.0f, 1.0f, 0.0f);
	if (fabsf(v3_dot(hp->axis, up)) > 0.999f)
		up = v3(1.0f, 0.0f, 0.0f);
	hp->u = v3_norm(v3_cross(up, hp->axis));
	hp->v = v3_norm(v3_cross(hp->axis, hp->u));
	// Use full height as the half-height clamp to avoid overly tight vertical clipping
	// This better matches the intended "Pringles" shape extent along the axis
	hp->half_height = hp->height;
	hp->inv_rx2 = 1.0f / (hp->rx * hp->rx);
	hp->inv_ry2 = 1.0f / (hp->ry * hp->ry);
	hp->inv_height = 1.0f / hp->height;
}

t_parse_result	parse_hp(char **tok, int line, t_scene *scene)
{
	t_object		*obj;
	t_parse_result	result;
	int				cbcons;

	result = hp_create_object(tok, line, &obj);
	if (!result.ok)
		return (result);
	result = hp_parse_attributes(tok, line, obj);
	if (!result.ok)
		return (result);
	hp_finalize(&obj->u_obj.hp);
    obj->u_obj.hp.has_checker = 0;
    obj->u_obj.hp.checker_scale = 1.0f; // also used as UV scale for bump
    obj->u_obj.hp.has_bump = 0;
    obj->u_obj.hp.bump_strength = 0.0f;
    obj->u_obj.hp.bump = NULL;
    if (tok[7] && ft_strncmp(tok[7], "bm", 3) == 0)
    {
	if (!parse_optional_bump(tok, 7, &obj->u_obj.hp.has_bump,
		&obj->u_obj.hp.bump_strength, &obj->u_obj.hp.bump))
	    return (object_error(obj, line, "hp: invalid bump (bm <png> <strength>)"));
    }
    else
    {
	cbcons = parse_optional_checker(tok, 7, &obj->u_obj.hp.has_checker,
		&obj->u_obj.hp.checker_scale);
	if (!cbcons)
	    return (object_error(obj, line, "hp: invalid checker (cb <scale>)"));
    }
	scene_add_object(scene, obj);
	return (parse_ok());
}

// Format: tr x1,y1,z1 x2,y2,z2 x3,y3,z3 r,g,b
t_parse_result	parse_tr(char **tokens, int line, t_scene *scene)
{
	t_object	*obj;
	int			cbcons;
	t_vec3		e1;
	t_vec3		e2;

	if (!tokens[1] || !tokens[2] || !tokens[3] || !tokens[4])
		return (parse_error(line, "tr: invalid format"));
	obj = (t_object *)malloc(sizeof(t_object));
	if (!obj)
		return (parse_error(line, "tr: not enough memory"));
	obj->type = OBJ_TRIANGLE;
	if (!parse_vec3(tokens[1], &obj->u_obj.tr.a))
		return (object_error(obj, line, "tr: invalid vertex a"));
	if (!parse_vec3(tokens[2], &obj->u_obj.tr.b))
		return (object_error(obj, line, "tr: invalid vertex b"));
	if (!parse_vec3(tokens[3], &obj->u_obj.tr.c))
		return (object_error(obj, line, "tr: invalid vertex c"));
	if (!parse_color_255(tokens[4], &obj->u_obj.tr.color))
		return (object_error(obj, line, "tr: invalid color"));
	{
		e1 = v3_sub(obj->u_obj.tr.b, obj->u_obj.tr.a);
		e2 = v3_sub(obj->u_obj.tr.c, obj->u_obj.tr.a);
		obj->u_obj.tr.u = v3_norm(e1);
		obj->u_obj.tr.v = v3_norm(v3_sub(e2, v3_mul(obj->u_obj.tr.u, v3_dot(e2, obj->u_obj.tr.u))));
	}
    obj->u_obj.tr.has_checker = 0;
    obj->u_obj.tr.checker_scale = 1.0f;
    obj->u_obj.tr.has_bump = 0;
    obj->u_obj.tr.bump_strength = 0.0f;
    obj->u_obj.tr.bump = NULL;
    if (tokens[5] && ft_strncmp(tokens[5], "bm", 3) == 0)
    {
	if (!parse_optional_bump(tokens, 5, &obj->u_obj.tr.has_bump,
		&obj->u_obj.tr.bump_strength, &obj->u_obj.tr.bump))
	    return (object_error(obj, line, "tr: invalid bump (bm <png> <strength>)"));
    }
    else
    {
	cbcons = parse_optional_checker(tokens, 5, &obj->u_obj.tr.has_checker,
		&obj->u_obj.tr.checker_scale);
	if (!cbcons)
	    return (object_error(obj, line, "tr: invalid checker (cb <scale>)"));
    }
	obj->next = NULL;
	scene_add_object(scene, obj);
	return (parse_ok());
}
/*
* Purpose: Parse a triangle primitive from three 3D points and an RGB color.
* Notes: This is an extension to support models converted by obj_to_rt.
*/
