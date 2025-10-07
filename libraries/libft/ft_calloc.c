/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 20:51:30 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/03 20:51:45 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	char	*p;
	size_t	s;

	s = nmemb * size;
	p = malloc(s);
	if (p == NULL)
		return (NULL);
	else
		ft_bzero(p, s);
	return (p);
}
// #include <stdio.h>
// #include <stdlib.h>

// int main()
// {
//     size_t nmemb = 5;
//     size_t size = sizeof(int);
//     int *arr1;
//     int *arr2;

//     printf("ft_calloc vs. calloc\n");
//     printf("Number of elements: %zu\n", nmemb);
//     printf("Size of each element: %zu\n", size);
//     arr1 = (int *)calloc(nmemb, size);
//     arr2 = (int *)ft_calloc(nmemb, size);
//     if (arr1 == NULL || arr2 == NULL)
// 	{
//         printf("Memory allocation failed\n");
//         return (1);
//     }
// }
