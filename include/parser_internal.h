#ifndef PARSER_INTERNAL_H
# define PARSER_INTERNAL_H

# include <stddef.h>
# include <stdbool.h>
# include "parser.h"

char			**read_file_lines(const char *path, size_t *out_count);
void			free_lines(char **lines, size_t count);
char			**split_ws(const char *s);
void			free_tokens(char **toks);
bool			parse_float(const char *s, float *out);
bool			parse_int_in_range(const char *s, int minv, int maxv, int *out);
bool			parse_vec3(const char *s, t_vec3 *out);
bool			parse_color_255(const char *s, t_vec3 *out);
bool			vec3_is_normalized(t_vec3 v);
bool			vec3_components_in_range(t_vec3 v, float minv, float maxv);
t_parse_result	parse_error(int line, const char *msg);
t_parse_result	parse_ok(void);
t_parse_result	dispatch_tokens(char **tokens, int line, t_scene *scene);

t_parse_result	parse_a(char **tokens, int line, t_scene *scene);
t_parse_result	parse_c(char **tokens, int line, t_scene *scene);
t_parse_result	parse_l(char **tokens, int line, t_scene *scene);
t_parse_result	parse_sp(char **tokens, int line, t_scene *scene);
t_parse_result	parse_pl(char **tokens, int line, t_scene *scene);
t_parse_result	parse_cy(char **tokens, int line, t_scene *scene);

#endif
