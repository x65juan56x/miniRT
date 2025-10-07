/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 19:06:04 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/03 19:06:07 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	char	*p;
	size_t	i;

	p = (char *)s;
	i = 0;
	while (i < n)
	{
		if ((unsigned char)p[i] == (unsigned char)c)
			return ((char *)s + i);
		i++;
	}
	return (NULL);
}
// #include <stdio.h>
// #include <string.h>

// int main()
// {
// 	const char str[] = "Hello, world!";
// 	int c = 'o';
// 	size_t n = 10;
// 	char *result1;
// 	char *result2;

// 	printf("ft_memchr vs. memchr\n");
// 	printf("String: '%s'\n", str);
// 	printf("Character to find: '%c'\n", c);
// 	printf("Number of bytes to search: %zu\n", n);
// 	result1 = memchr(str, c, n);
// 	result2 = ft_memchr(str, c, n);
// 	printf("Original memchr: %s\n", result1 ? result1 : "Not found");
// 	printf("libft ft_memchr: %s\n", result2 ? result2 : "Not found");
// 	return (0);
// }
