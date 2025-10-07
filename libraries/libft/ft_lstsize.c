/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:29:36 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/14 14:02:59 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	size_t	n;

	n = 0;
	while (lst)
	{
		lst = lst->next;
		n++;
	}
	return (n);
}
/*
Line 10: Update the "lst" pointer to point to the next node in the list
    (lst->next). The "lst" variable is a local pointer in the function.
    When "lst" is updated to "lst->next", it only changes the address to
    which "lst" points within the function's context. This does not alter
    the original nodes or the list structure outside this function. "lst"
    is a copy of the pointer passed to the function. Modifying "lst"
    within the function does not affect the original pointer that points
    to the first node of the list outside the function.
*/

// #include <stdio.h>

// int main()
// {
// 	int value1 = 42;
// 	int value2 = 21;
// 	int value3 = 84;
// 	t_list *head;
// 	t_list *second;
// 	t_list *third;
// 	int size;

// 	printf("ft_lstsize\n");
// 	head = ft_lstnew(&value1);
// 	second = ft_lstnew(&value2);
// 	third = ft_lstnew(&value3);
// 	head->next = second;
// 	second->next = third;
// 	size = ft_lstsize(head);
// 	printf("Size of the list: %d\n", size);
// 	free(third);
// 	free(second);
// 	free(head);
// 	return (0);
// }
