/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:29:06 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/19 20:20:10 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*newl;
	t_list	*c;
	void	*content;

	if (!lst || !f)
		return (NULL);
	newl = 0;
	while (lst)
	{
		content = f(lst->content);
		c = ft_lstnew(content);
		if (!c)
		{
			del(content);
			ft_lstclear(&newl, del);
			return (NULL);
		}
		ft_lstadd_back(&newl, c);
		lst = lst->next;
	}
	return (newl);
}
/*
Line 23: Initialize the new list to "0" to clear any unnecessary data.
Line 26: Apply function "f" to original node's content (lst->content) and
    create a new node with the result using ft_lstnew, storing it in "c".
Line 28: If creating the new element fails, free all allocated memory in "newl"
    using ft_lstclear and the "del" function.
Line 34: Add the new node "c" to the end of the new list "newl".
Line 35: Advance to the next node.
*/

/*
#include <stdio.h>

// Example function to map content
void *example_func(void *content)
{
	int *new_content = malloc(sizeof(int));
	if (new_content == NULL)
	{
		return NULL;
	}
	*new_content = *(int *)content * 2; // Example: double the value
	return new_content;
}

// Example function to delete content
void del(void *content)
{
	free(content);
}

int main()
{
	int *value1 = malloc(sizeof(int));
	int *value2 = malloc(sizeof(int));
	int *value3 = malloc(sizeof(int));
	if (value1 == NULL || value2 == NULL || value3 == NULL)
	{
		perror("Error allocating memory");
		return (1);
	}
	*value1 = 42;
	*value2 = 21;
	*value3 = 84;
	t_list *head;
	t_list *second;
	t_list *third;
	t_list *new_list;

	printf("ft_lstmap\n");

	// Create list nodes
	head = ft_lstnew(value1);
	second = ft_lstnew(value2);
	third = ft_lstnew(value3);

	// Link the nodes
	head->next = second;
	second->next = third;

	// Map the list nodes
	new_list = ft_lstmap(head, example_func, del);

	// Print the new list content
	t_list *current = new_list;
	while (current != NULL)
	{
		printf("New node content: %d\n", *(int *)(current->content));
		current = current->next;
	}

	// Free the allocated memory
	ft_lstclear(&new_list, del);
	ft_lstclear(&head, del);

	return (0);
	}
	*/
