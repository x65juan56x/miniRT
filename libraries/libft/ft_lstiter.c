/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstiter.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:27:14 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/12 12:27:16 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstiter(t_list *lst, void (*f)(void *))
{
	if (!lst || !f)
		return ;
	while (lst)
	{
		f(lst->content);
		lst = lst->next;
	}
}
// #include <stdio.h>
// #include <stdlib.h>

// // Función de ejemplo para iterar sobre el contenido
// void example_func(void *content) {
//     printf("Node content: %d\n", *(int *)content);
// }

// int main()
// {
// 	int *value1 = malloc(sizeof(int));
// 	int *value2 = malloc(sizeof(int));
// 	int *value3 = malloc(sizeof(int));
// 	if (value1 == NULL || value2 == NULL || value3 == NULL)
// 	{
// 		perror("Error allocating memory");
// 		return 1;
// 	}
// 	*value1 = 42;
// 	*value2 = 21;
// 	*value3 = 84;
// 	t_list *head;
// 	t_list *second;
// 	t_list *third;

// 	printf("ft_lstiter\n");
// 	head = ft_lstnew(value1);
// 	second = ft_lstnew(value2);
// 	third = ft_lstnew(value3);
// 	head->next = second;
// 	second->next = third;
// 	ft_lstiter(head, example_func);
// 	free(third);
// 	free(second);
// 	free(head);
// 	return (0);
// }
