#include <stdlib.h>
#include "../../include_bonus/parser_internal_bonus.h"

void	free_lines(char **lines, size_t count)
{
	size_t	index;

	if (!lines)
		return ;
	index = 0;
	while (index < count)
	{
		free(lines[index]);
		index++;
	}
	free(lines);
}
/*
* Purpose: Free the array of strings produced by `read_file_lines`.
* Notes: Iterates over the recorded count before releasing the pointer array.
*/
