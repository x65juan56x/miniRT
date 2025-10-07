/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 21:47:53 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 21:47:57 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	ft_memset(s, '\0', n);
}
// #include <stdio.h>
// #include <string.h>

// int main()
// {
// 	char str1[50] = "This is a string";
// 	char str2[50] = "This is a string";
// 	size_t n = 5;

// 	printf("ft_bzero vs. bzero\n");
// 	printf("Original string: '%s'\n", str1);
// 	bzero(str1, n);
// 	ft_bzero(str2, n);
// 	printf("Original bzero: '%s'\n", str1);
// 	printf("libft ft_bzero: '%s'\n", str2);
// 	return (0);
// }
