/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 19:36:24 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/03 19:36:29 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	char	*p;
	char	*q;
	size_t	i;

	p = (char *)s1;
	q = (char *)s2;
	i = 0;
	while (i < n)
	{
		if (p[i] != q[i])
			return ((unsigned char)p[i] - (unsigned char)q[i]);
		i++;
	}
	return (0);
}
// #include <stdio.h>
// #include <string.h>

// int main() 
// {
// 	const char *s1 = "Hello, world!";
// 	const char *s2 = "Hello, there!";
// 	size_t n = 10;

// 	printf("ft_memcmp vs. memcmp\n");
// 	printf("String 1: '%s'\n", s1);
// 	printf("String 2: '%s'\n", s2);
// 	printf("Number of bytes to compare: %zu\n", n);
// 	int result1 = memcmp(s1, s2, n);
// 	int result2 = ft_memcmp(s1, s2, n);
// 	printf("Original memcmp: %d\n", result1);
// 	printf("libft ft_memcmp: %d\n", result2);
// 	return (0);
// }
