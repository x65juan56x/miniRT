/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 16:53:04 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/16 21:06:55 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

char	*ft_get_line(t_list *frst_nd)
{
	size_t	ln_len;
	t_list	*aux_nd;
	char	*line;
	size_t	i;
	size_t	j;

	aux_nd = frst_nd;
	ln_len = ft_ln_len(frst_nd);
	if (ln_len <= 0)
		return (ft_free_lst(aux_nd));
	line = malloc(sizeof(char) * (ln_len + 1));
	if (!line)
		return (ft_free_lst(aux_nd));
	frst_nd = aux_nd;
	i = 0;
	while (frst_nd && i < ln_len)
	{
		j = 0;
		while ((frst_nd->str)[j])
			line[i++] = (frst_nd->str)[j++];
		frst_nd = frst_nd->next;
	}
	line[i] = '\0';
	ft_free_lst(aux_nd);
	return (line);
}

static void	ft_fill_remainder_str(char *dest_str, char *src_nl_ptr)
{
	size_t	src_i;
	size_t	dst_i;

	src_i = 1;
	dst_i = 0;
	while (src_nl_ptr[src_i] && dst_i < BUFFER_SIZE)
	{
		dest_str[dst_i] = src_nl_ptr[src_i];
		dst_i++;
		src_i++;
	}
	dest_str[dst_i] = '\0';
}

t_list	*ft_get_tail(t_list *list_arr[MAX_FD], char *nl_ptr_in_node_str, int fd)
{
	t_list	*new_tail_node;

	if (!list_arr[fd] || !nl_ptr_in_node_str || nl_ptr_in_node_str[0] != '\n')
		return (NULL);
	if (nl_ptr_in_node_str[1] == '\0')
		return (NULL);
	new_tail_node = malloc(sizeof(t_list));
	if (!new_tail_node)
	{
		nl_ptr_in_node_str[1] = '\0';
		return (NULL);
	}
	new_tail_node->next = NULL;
	ft_fill_remainder_str(new_tail_node->str, nl_ptr_in_node_str);
	nl_ptr_in_node_str[1] = '\0';
	if (new_tail_node->str[0] == '\0')
	{
		free(new_tail_node);
		return (NULL);
	}
	return (new_tail_node);
}

int	ft_read_til_nl(char **nl_pos, t_list **frst_nd, int fd, t_list **list)
{
	int	rd_chr;

	rd_chr = 1;
	*nl_pos = NULL;
	*frst_nd = NULL;
	if (list[fd] != NULL)
	{
		*frst_nd = list[fd];
		*nl_pos = ft_pnt_nl(list[fd]->str);
	}
	while (*nl_pos == NULL && rd_chr > 0)
	{
		if (!ft_fwd_lst(list, frst_nd, fd))
			return (-1);
		rd_chr = read(fd, list[fd]->str, BUFFER_SIZE);
		if (rd_chr < 0)
		{
			list[fd] = NULL;
			ft_free_lst(*frst_nd);
			return (-1);
		}
		(list[fd]->str)[rd_chr] = '\0';
		*nl_pos = ft_pnt_nl(list[fd]->str);
	}
	return (rd_chr);
}

char	*get_next_line(int fd)
{
	static t_list	*list[MAX_FD];
	t_list			*frst_nd;
	char			*nl_pos;
	int				rd_chr;

	if (fd < 0 || fd > (MAX_FD - 1) || BUFFER_SIZE <= 0)
		return (NULL);
	rd_chr = ft_read_til_nl(&nl_pos, &frst_nd, fd, list);
	if (rd_chr < 0)
		return (NULL);
	list[fd] = ft_get_tail(list, nl_pos, fd);
	return (ft_get_line(frst_nd));
}
/*
Managing multiple file descriptors (fd) is handled using a static array of
linked lists.
Each index of the array corresponds to a specific file descriptor, allowing the
program to maintain a separate state for each file being read.

The maximum number of 4096 file descriptors (fd) in the code is based on the
typical limit of file descriptors a process can handle in Unix and Linux
systems. This limit may vary depending on system configuration and program
needs.
*/
