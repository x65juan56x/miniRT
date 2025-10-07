/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 12:24:34 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/12 12:24:37 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_front(t_list **lst, t_list *new)
{
	if (!new)
		return ;
	if (!lst)
	{
		*lst = new;
		return ;
	}
	new->next = *lst;
	*lst = new;
}
/*
Linea 6: chequea si el puntero "new" es nulo. Si es nulo, no hay nada que
	añadir, entonces la función retorna sin hacer nada.
Linea 8: chequea si "lst" es nulo. Esto verifica si el puntero a la lista
	misma es nulo, no solo si la lista está vacía. Si "lst" es nulo, asigna
	"new" directamente a "*lst". Esto inicializa la lista con "new" como el
	único nodo. Después de asignar new a una lista inicialmente NULL, la
	función termina.
Linea 13: Si la lista ya existe ("lst" no es nulo), este paso asigna "*lst"
	(el primer nodo actual de la lista) al puntero "next" del nuevo nodo "new".
	Esto enlaza el nuevo nodo al principio de la lista.
Linea 14: "new" se convierte en el nuevo primer nodo de la lista. "*lst" se
	actualiza para que apunte a "new".

"lst" es un doble puntero (t_list **lst) porque queremos modificar la dirección
del puntero que apunta al primer nodo de la lista dentro de la función.
Cuando pasamos un puntero (t_list *), pasamos una copia de la dirección del
primer nodo. Cualquier cambio que hagamos afecta solo a esa copia y no al
puntero original.
Al pasar un doble puntero (t_list **lst), tenemos acceso a la dirección del
puntero original y podemos modificarlo directamente desde dentro de la función.
*/
// #include <stdio.h>

// int main()
// {
// 	int value1 = 42;
// 	int value2 = 21;
// 	t_list *head;
// 	t_list *new_node;

// 	printf("ft_lstadd_front\n");

// 	// Crear el primer nodo
// 	head = ft_lstnew(&value1);
// 	if (head != NULL) {
// 		printf("Initial head node created successfully\n");
// 		printf("Head node content: %d\n", *(int *)(head->content));
// 	} else {
// 		printf("ft_lstnew returned NULL for head node\n");
// 	}

// 	// Crear el nuevo nodo
// 	new_node = ft_lstnew(&value2);
// 	if (new_node != NULL) {
// 		printf("New node created successfully\n");
// 		printf("New node content: %d\n", *(int *)(new_node->content));
// 	} else {
// 		printf("ft_lstnew returned NULL for new node\n");
// 	}

// 	// Añadir el nuevo nodo al frente de la lista
// 	ft_lstadd_front(&head, new_node);
// 	printf("Added new node to the front of the list\n");
// 	printf("Head node content after add: %d\n", *(int *)(head->content));
// 	printf("Next node content after add: %d\n", *(int *)(head->next->content));

// 	// Liberar la memoria asignada
// 	free(new_node);
// 	free(head);

// 	return 0;
// }
