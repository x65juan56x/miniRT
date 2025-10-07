/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 16:53:52 by jmondon           #+#    #+#             */
/*   Updated: 2025/07/21 11:09:21 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*ft_pnt_nl(char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '\n')
			return (str + i);
		i++;
	}
	return (NULL);
}

int	ft_fwd_lst(t_list_gnl **list_static_ptr, t_list_gnl **frst_nd_head_ptr)
{
	t_list_gnl	*new_node;

	new_node = malloc(sizeof(t_list_gnl));
	if (!new_node)
	{
		ft_free_lst(*frst_nd_head_ptr);
		*frst_nd_head_ptr = NULL;
		*list_static_ptr = NULL;
		return (0);
	}
	new_node->next = NULL;
	if (*list_static_ptr == NULL)
		*frst_nd_head_ptr = new_node;
	else
		(*list_static_ptr)->next = new_node;
	*list_static_ptr = new_node;
	return (1);
}

void	*ft_free_lst(t_list_gnl *list)
{
	t_list_gnl	*aux_nd;

	while (list)
	{
		aux_nd = (list)->next;
		free(list);
		list = aux_nd;
	}
	return (NULL);
}

size_t	ft_str_len(const char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

size_t	ft_ln_len(t_list_gnl *list)
{
	size_t	l;

	l = 0;
	while (list)
	{
		l += ft_str_len(list->str);
		list = list->next;
	}
	return (l);
}
/*
ft_fwd_lst:
	Creates a new node (new). If the list is empty, assigns the node to frst_nd
	and also to the list (line 44). If the list is not empty, adds "new" to the
	end of the list and then moves the list pointer to the new node (line 44).

ft_pnt_nl:
	Receives a pointer to a string and returns a pointer to the position of the
	newline (\n) in the string. If it doesn't find the newline, it returns NULL.

ft_free_lst:
	Receives a pointer to a node in a linked list and frees the memory of that
	node and all subsequent nodes.

ft_ln_len:
	Receives a pointer to the first node in a linked list and returns the sum of
	the lengths of all the strings in the nodes of the list.

ft_str_len:
	Receives a pointer to a string and returns the length of the string.
*/
