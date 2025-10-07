/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 16:53:43 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/16 19:12:54 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

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

int	ft_fwd_lst(t_list *list_arr[MAX_FD], t_list **frst_nd_head_ptr, int fd)
{
	t_list	*new_node;

	new_node = malloc(sizeof(t_list));
	if (!new_node)
	{
		ft_free_lst(*frst_nd_head_ptr);
		*frst_nd_head_ptr = NULL;
		list_arr[fd] = NULL;
		return (0);
	}
	new_node->str[0] = '\0';
	new_node->next = NULL;
	if (list_arr[fd] == NULL)
		*frst_nd_head_ptr = new_node;
	else
		list_arr[fd]->next = new_node;
	list_arr[fd] = new_node;
	return (1);
}

void	*ft_free_lst(t_list *list)
{
	t_list	*aux_nd;

	while (list)
	{
		aux_nd = list->next;
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

size_t	ft_ln_len(t_list *list)
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
