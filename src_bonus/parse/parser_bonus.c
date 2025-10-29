#include "../../include_bonus/parser_internal_bonus.h"
#include "../../libraries/libft/libft.h"

static int	validate_extension(const char *path)
{
	size_t	len;

	len = ft_strlen(path);
	if (len < 3)
		return (0);
	if (ft_strncmp(path + len - 3, ".rt", 3) != 0)
		return (0);
	return (1);
}
/*
* Purpose: Ensure the provided path ends with the mandatory ".rt" extension.
* Returns: 1 on success, 0 when the file name is too short or mismatched.
*/

static char	*prepare_line(char *line)
{
	size_t	index;
	char	*hash;

	index = 0;
	while (line[index] == ' ' || line[index] == '\t')
		index++;
	hash = ft_strchr(line + index, '#');
	if (hash)
		*hash = '\0';
	return (line + index);
}
/*
* Purpose: Trim leading whitespace and ignore inline comments starting with '#'.
* Returns: Pointer to the first significant character within the original line.
*/

static t_parse_result	process_line(char *line, size_t index, t_scene *scene)
{
	char			**tokens;
	t_parse_result	result;

	tokens = split_ws(line);
	if (!tokens)
		return (parse_error((int)(index + 1), "split failed"));
	if (!tokens[0])
	{
		free_tokens(tokens);
		return (parse_ok());
	}
	result = dispatch_tokens(tokens, (int)(index + 1), scene);
	free_tokens(tokens);
	return (result);
}
/*
* Purpose: Tokenize a meaningful line and dispatch it to the appropriate parser.
* Errors: Converts allocation issues or parsing failures into structured results.
*/

static t_parse_result	parse_lines(char **lines, size_t count, t_scene *scene)
{
	size_t			index;
	char			*trimmed;
	t_parse_result	result;

	index = 0;
	while (index < count)
	{
		trimmed = prepare_line(lines[index]);
		if (*trimmed != '\0')
		{
			result = process_line(trimmed, index, scene);
			if (!result.ok)
				return (result);
		}
		index++;
	}
	return (parse_ok());
}
/*
* Purpose: Iterate over every line of the input file,
* skipping blanks and comments.
* Workflow: Normalizes each line, delegates meaningful ones,
* and stops on errors.
*/

t_parse_result	parse_scene(const char *path, t_scene *scene)
{
	char			**lines;
	size_t			count;
	t_parse_result	result;

	if (!validate_extension(path))
		return (parse_error(0, "The file does not have .rt extension"));
	scene_init(scene);
	count = 0;
	lines = read_file_lines(path, &count);
	if (!lines)
		return (parse_error(0, "Unable to open/read file"));
	result = parse_lines(lines, count, scene);
	if (result.ok)
	{
		if (!scene->ambient.present)
			result = parse_error(0, "Missing Ambient (A)");
		else if (!scene->camera.present)
			result = parse_error(0, "Missing Camera (C)");
		else if (!scene->light)
			result = parse_error(0, "Missing Light (L)");
	}
	if (!result.ok)
		scene_free(scene);
	free_lines(lines, count);
	return (result);
}
/*
* Purpose: Public entry point that validates the file, reads it,
* and builds the scene.
* Guarantees: Initializes the scene, enforces required entities,
* and cleans on failure.
*/
