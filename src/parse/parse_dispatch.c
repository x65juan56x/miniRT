#include "../../libraries/libft/libft.h"
#include "../../include/parser_internal.h"

t_parse_result	dispatch_tokens(char **tokens, int line, t_scene *scene)
{
	if (ft_strncmp(tokens[0], "A", 2) == 0)
		return (parse_a(tokens, line, scene));
	if (ft_strncmp(tokens[0], "C", 2) == 0)
		return (parse_c(tokens, line, scene));
	if (ft_strncmp(tokens[0], "L", 2) == 0)
		return (parse_l(tokens, line, scene));
	if (ft_strncmp(tokens[0], "sp", 3) == 0)
		return (parse_sp(tokens, line, scene));
	if (ft_strncmp(tokens[0], "pl", 3) == 0)
		return (parse_pl(tokens, line, scene));
	if (ft_strncmp(tokens[0], "cy", 3) == 0)
		return (parse_cy(tokens, line, scene));
	return (parse_error(line, "Unknown identifier"));
}
/*
* Purpose: Route the first token of a scene line to the correct parsing routine.
* Behavior: Matches identifiers for ambient, camera, light, and object entries.
* Failure: Produces a descriptive error when an unknown keyword is encountered.
*/
