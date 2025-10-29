#include <stdlib.h>
#include "../../libraries/libft/libft.h"
#include "../../include_bonus/parser_internal_bonus.h"

t_parse_result	parse_error(int line, const char *msg)
{
	t_parse_result	result;
	size_t			len;

	result.ok = false;
	result.message = NULL;
	result.line = line;
	len = ft_strlen(msg) + 1;
	result.message = (char *)malloc(len);
	if (result.message)
		ft_memcpy(result.message, msg, len);
	return (result);
}
/*
* Purpose: Build a failure result, copying the provided message for reporting.
* Notes: Stores the original line number and allocates a writable string.
*/

t_parse_result	parse_ok(void)
{
	t_parse_result	result;

	result.ok = true;
	result.message = NULL;
	result.line = 0;
	return (result);
}
/*
* Purpose: Produce a success descriptor with no error message attached.
* Use: Returned by parsing helpers when everything validates correctly.
*/

void	parse_result_free(t_parse_result *r)
{
	if (!r)
		return ;
	if (r->message)
	{
		free(r->message);
		r->message = NULL;
	}
}
/*
* Purpose: Release the heap-allocated message contained in a parse result.
* Guards: Accepts NULL pointers and silently handles already-freed strings.
*/
