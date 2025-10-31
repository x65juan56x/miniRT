#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include_bonus/parser_internal_bonus.h"

static char	*dup_with_spaces(const char *src)
{
	size_t	len;
	char	*dst;
	size_t	index;

	len = ft_strlen(src);
	dst = (char *)malloc(len + 1);
	if (!dst)
		return (NULL);
	index = 0;
	while (index < len)
	{
		if (src[index] == '\t')
			dst[index] = ' ';
		else
			dst[index] = src[index];
		index++;
	}
	dst[index] = '\0';
	return (dst);
}
/*
* Purpose: Duplicate a string while normalizing tabs into spaces for splitting.
* Notes: Keeps other characters intact; caller must free the returned copy.
*/

char	**split_ws(const char *s)
{
	char	*norm;
	char	**tokens;

	norm = dup_with_spaces(s);
	if (!norm)
		return (NULL);
	tokens = ft_split(norm, ' ');
	free(norm);
	return (tokens);
}
/*
* Purpose: Produce a NULL-terminated token array split on spaces.
* Handling: Converts tabs to spaces first.
*/

void	free_tokens(char **toks)
{
	size_t	index;

	if (!toks)
		return ;
	index = 0;
	while (toks[index])
	{
		free(toks[index]);
		index++;
	}
	free(toks);
}
/*
* Purpose: Release the token array created by `split_ws`.
* Behavior: Frees each string element and the container pointer.
*/
