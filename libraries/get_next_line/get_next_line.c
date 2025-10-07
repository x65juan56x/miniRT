/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 16:54:03 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/01 19:07:51 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*ft_get_line(t_list_gnl *frst_nd)
{
	size_t		ln_len;
	size_t		i;
	size_t		j;
	t_list_gnl	*aux_nd;
	char		*line;

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
	line[i - 1] = '\0';
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

t_list_gnl	*ft_get_tail(t_list_gnl *node_with_nl, char *nl_ptr_in_node_str)
{
	t_list_gnl	*new_tail_node;

	if (!node_with_nl || !nl_ptr_in_node_str || nl_ptr_in_node_str[0] != '\n')
		return (NULL);
	if (nl_ptr_in_node_str[1] == '\0')
		return (NULL);
	new_tail_node = malloc(sizeof(t_list_gnl));
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

int	ft_read_til_nl(char **nl_pos, t_list_gnl **frst_nd, int fd,
		t_list_gnl **list)
{
	int	rd_chr;

	rd_chr = 1;
	*nl_pos = NULL;
	*frst_nd = NULL;
	if (*list != NULL)
	{
		*frst_nd = *list;
		*nl_pos = ft_pnt_nl((*list)->str);
	}
	while (*nl_pos == NULL && rd_chr > 0)
	{
		if (!ft_fwd_lst(list, frst_nd))
			return (-1);
		rd_chr = read(fd, (*list)->str, BUFFER_SIZE);
		if (rd_chr < 0)
		{
			*list = NULL;
			ft_free_lst(*frst_nd);
			return (-1);
		}
		((*list)->str)[rd_chr] = '\0';
		*nl_pos = ft_pnt_nl((*list)->str);
	}
	return (rd_chr);
}

char	*get_next_line(int fd)
{
	static t_list_gnl	*list_static = NULL;
	t_list_gnl			*frst_nd_cur_line;
	char				*nl_pos_in_cur_node;
	int					rd_chr;
	t_list_gnl			*new_tail;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	rd_chr = ft_read_til_nl(&nl_pos_in_cur_node, &frst_nd_cur_line, fd,
			&list_static);
	if (rd_chr < 0)
		return (NULL);
	new_tail = ft_get_tail(list_static, nl_pos_in_cur_node);
	list_static = new_tail;
	return (ft_get_line(frst_nd_cur_line));
}
