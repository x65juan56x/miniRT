/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 22:08:11 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/10 22:08:13 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*new;

	new = (t_list *)malloc(sizeof(t_list));
	if (!new)
		return (NULL);
	new->content = content;
	new->next = NULL;
	return (new);
}
// #include <stdio.h>

// int main()
// {
// 	int value = 42;
// 	t_list *node;

// 	printf("ft_lstnew\n");
// 	printf("Content to add: %d\n", value);
// 	node = ft_lstnew(&value);
// 	if (node != NULL) {
// 		printf("Node created successfully\n");
// 		printf("Node content: %d\n", *(int *)(node->content));
// 		printf("Node next: %p\n", (void *)node->next);
// 		free(node);
// 	}
// 	else
// 		printf("ft_lstnew returned NULL\n");
// 	return (0);
// }
