#include <math.h>
#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include_bonus/parser_internal_bonus.h"
#include "../../include_bonus/bump_bonus.h"
#include "../../include_bonus/scene_bonus.h"

t_parse_result	obj_error(t_object *obj, int line, const char *msg)
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

void	free_sp_pl_cy_with_addons(t_object *obj)
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

void	free_tr_hp_with_addons(t_object *obj)
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

// - idx: position where "cb" could appear
// - Returns 1 if valid (present or absent), 0 if format is invalid
// - If present, validates "cb <scale>" and lets the caller advance the index
int	parse_opt_checker(char **tokens, int idx, int *has_checker,
		float *out_scale)
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

int	parse_opt_bump(char **tokens, int idx, t_bump_target target)
{
	*target.has_bump = 0;
	*target.map = NULL;
	if (!tokens[idx])
		return (1);
	if (ft_strncmp(tokens[idx], "bm", 3) != 0)
		return (1);
	if (!tokens[idx + 1] || !tokens[idx + 2])
		return (0);
	if (!parse_float(tokens[idx + 2], target.strength)
		|| *target.strength < 0.0f)
		return (0);
	*target.map = bump_load_png(tokens[idx + 1]);
	if (!*target.map)
		return (0);
	*target.has_bump = 1;
	return (1);
}
