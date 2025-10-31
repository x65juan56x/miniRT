#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../libraries/libft/libft.h"
#include "../../libraries/get_next_line/get_next_line.h"
#include "../../include/parser_internal.h"

static void	chomp_line(char *line)
{
	size_t	len;

	len = ft_strlen(line);
	while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
	{
		len--;
		line[len] = '\0';
	}
}
/*
* Purpose: Strip trailing newline and carriage-return characters from a line.
* Context: Ensures parser logic works with clean strings from get_next_line.
*/

static int	append_line(t_list **lst, char *line)
{
	t_list	*node;

	node = ft_lstnew(line);
	if (!node)
		return (0);
	ft_lstadd_back(lst, node);
	return (1);
}
/*
* Purpose: Push a freshly read line into a temporary linked list.
* Returns: 1 on success, 0 on allocation failure (caller cleans up).
*/

static int	read_lines_fd(int fd, t_list **lst, size_t *count)
{
	char	*line;

	line = get_next_line(fd);
	while (line)
	{
		chomp_line(line);
		if (!append_line(lst, line))
		{
			free(line);
			return (0);
		}
		*count = *count + 1;
		line = get_next_line(fd);
	}
	return (1);
}
/*
* Purpose: Load every line from the file descriptor into a list of strings.
* Behavior: Tracks the total count and stops gracefully on allocation errors.
*/

static char	**list_to_array(t_list *lst, size_t count)
{
	char	**lines;
	size_t	index;

	lines = (char **)malloc(sizeof(char *) * (count + 1));
	if (!lines)
		return (NULL);
	index = 0;
	while (index < count)
	{
		lines[index] = ft_strdup((char *)lst->content);
		if (!lines[index])
		{
			while (index > 0)
			{
				index--;
				free(lines[index]);
			}
			free(lines);
			return (NULL);
		}
		lst = lst->next;
		index++;
	}
	lines[count] = NULL;
	return (lines);
}
/*
* Purpose: Materialize the linked-list buffer into a contiguous array of lines.
* Guarantees: Duplicates each string and frees partial allocations on failure.
*/

char	**read_file_lines(const char *path, size_t *out_count)
{
	int		fd;
	t_list	*lst;
	size_t	count;
	char	**lines;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (NULL);
	lst = NULL;
	count = 0;
	if (!read_lines_fd(fd, &lst, &count))
	{
		close(fd);
		ft_lstclear(&lst, free);
		return (NULL);
	}
	close(fd);
	lines = list_to_array(lst, count);
	ft_lstclear(&lst, free);
	if (!lines)
		return (NULL);
	if (out_count)
		*out_count = count;
	return (lines);
}
/*
* Purpose: Read an entire file into a NULL-terminated array of strings.
* Outcome: Provides ownership of the array to the caller with optional count.
* Failure: Returns NULL and leaves cleanup to the caller.
*/
