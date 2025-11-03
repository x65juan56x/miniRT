#include <math.h>
#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include_bonus/parser_internal_bonus.h"
#include "../../include_bonus/bump_bonus.h"
#include "../../include_bonus/scene_bonus.h"

static void	spec_state_init(t_spec_state *st, char **tokens,
		int line, const char *tag)
{
	st->tokens = tokens;
	st->line = line;
	st->tag = tag;
	st->idx = 0;
	st->has_any = 0;
	st->has_ks = 0;
	st->ks = 0.0f;
	st->shininess = 0.0f;
	st->reflectivity = 0.0f;
	st->model = SPEC_MODEL_BLINN;
}

static void	spec_parse_model(t_spec_state *st)
{
	char	**tok;
	char	*cur;

	tok = st->tokens;
	cur = tok[st->idx];
	if (!cur)
		return ;
	if (ft_strncmp(cur, "phong", 6) == 0)
	{
		st->model = SPEC_MODEL_PHONG;
		st->idx++;
	}
	else if (ft_strncmp(cur, "blinn", 6) == 0
		|| ft_strncmp(cur, "blinn-phong", 12) == 0)
	{
		st->model = SPEC_MODEL_BLINN;
		st->idx++;
	}
}

static t_parse_result	spec_parse_ks(t_spec_state *st)
{
	char	**tok;

	tok = st->tokens;
	if (!tok[st->idx] || ft_strncmp(tok[st->idx], "ks", 3) != 0)
		return (parse_ok());
	st->has_any = 1;
	st->has_ks = 1;
	if (!tok[st->idx + 1] || !tok[st->idx + 2])
		return (spec_error(st->line, st->tag,
				"material expects 'ks <ratio> <shininess>'"));
	if (!parse_float(tok[st->idx + 1], &st->ks)
		|| st->ks < 0.0f || st->ks > 1.0f)
		return (spec_error(st->line, st->tag,
				"invalid ks (use 0 <= ks <= 1)"));
	if (!parse_float(tok[st->idx + 2], &st->shininess)
		|| st->shininess <= 0.0f)
		return (spec_error(st->line, st->tag,
				"invalid shininess (> 0 required)"));
	st->idx += 3;
	spec_parse_model(st);
	return (parse_ok());
}

static t_parse_result	spec_parse_kr(t_spec_state *st)
{
	char	**tok;

	tok = st->tokens;
	if (!tok[st->idx])
		return (parse_ok());
	if (ft_strncmp(tok[st->idx], "kr", 3) != 0)
	{
		if (st->has_ks)
			return (spec_error(st->line, st->tag,
					"unexpected tokens after material definition"));
		return (spec_error(st->line, st->tag,
				"unknown material token (ks|kr)"));
	}
	if (!tok[st->idx + 1])
		return (spec_error(st->line, st->tag,
				"kr expects '<ratio>'"));
	if (!parse_float(tok[st->idx + 1], &st->reflectivity)
		|| st->reflectivity < 0.0f || st->reflectivity > 1.0f)
		return (spec_error(st->line, st->tag,
				"invalid kr (use 0 <= kr <= 1)"));
	st->has_any = 1;
	st->idx += 2;
	return (parse_ok());
}

t_parse_result	parse_specular_info(char **tokens, int line,
		const char *tag, t_material **out_mat)
{
	t_spec_state	st;
	t_parse_result	res;

	*out_mat = NULL;
	if (!tokens || !tokens[0])
		return (parse_ok());
	spec_state_init(&st, tokens, line, tag);
	res = spec_parse_ks(&st);
	if (!res.ok)
		return (res);
	res = spec_parse_kr(&st);
	if (!res.ok)
		return (res);
	res = spec_validate_tail(&st);
	if (!res.ok)
		return (res);
	return (spec_finalize_material(&st, out_mat));
}
