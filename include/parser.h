/*
* Parser API: parse a .rt scene file into a t_scene model.
* Provides a simple result struct for error reporting and helpers for tests.
*/
#ifndef PARSER_H
# define PARSER_H

# include <stdbool.h>
# include "scene.h"
/*
* Parsing result descriptor.
* ok: true on success, false on error.
* message: malloc'ed human-readable error (NULL on success);
* caller frees via parse_result_free.
* line: 1-based line number for the error
* (0 if not applicable, e.g., missing A/C/L).
*/
typedef struct s_parse_result
{
	bool	ok;
	char	*message; // malloc'd message on failure; NULL on success
	int		line; // 1-based line number for error; 0 if not applicable
}	t_parse_result;

/*
* Parse a .rt file at `path` and fill `out`.
* Returns a parse result (ok or error).
*/
t_parse_result	parse_scene(const char *path, t_scene *out);
/* Free resources in a parse result (message). */
void			parse_result_free(t_parse_result *r);

// Helpers from parser_utils.c used by parser.c
bool			vec3_components_in_range(t_vec3 v, float minv, float maxv);

#endif
