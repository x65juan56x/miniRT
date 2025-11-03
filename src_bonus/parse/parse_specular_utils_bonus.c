#include <math.h>
#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include_bonus/parser_internal_bonus.h"
#include "../../include_bonus/bump_bonus.h"
#include "../../include_bonus/scene_bonus.h"

t_parse_result	spec_validate_tail(t_spec_state *st)
{
	if (st->tokens[st->idx])
		return (spec_error(st->line, st->tag,
				"unexpected tokens after material definition"));
	return (parse_ok());
}

t_parse_result	spec_finalize_material(t_spec_state *st,
		t_material **out_mat)
{
	t_material	*mat;

	if (!st->has_any)
		return (parse_ok());
	mat = (t_material *)malloc(sizeof(*mat));
	if (!mat)
		return (spec_error(st->line, st->tag,
				"not enough memory for material"));
	mat->albedo = v3(0.0f, 0.0f, 0.0f);
	mat->ks = st->ks;
	mat->shininess = st->shininess;
	mat->model = st->model;
	mat->reflectivity = st->reflectivity;
	*out_mat = mat;
	return (parse_ok());
}

t_parse_result	spec_error(int line, const char *tag, const char *msg)
{
	char			*prefix;
	char			*full;
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
