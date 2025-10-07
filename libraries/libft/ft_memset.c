/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 21:37:56 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 21:37:59 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	char	*p;

	p = (char *)s;
	while (n > 0)
	{
		p[n - 1] = c;
		n--;
	}
	return (s);
}
// #include <stdio.h>
// #include <string.h>

// int main()
// {
// 	char str1[50] = "This is a string";
// 	char str2[50] = "This is a string";
// 	int c = '*';
// 	size_t n = 5;

// 	printf("ft_memset vs. memset\n");
// 	printf("Original string: '%s'\n", str1);
// 	memset(str1, c, n);
// 	ft_memset(str2, c, n);
// 	printf("Original memset: '%s'\n", str1);
// 	printf("libft ft_memset: '%s'\n", str2);
// 	return (0);
// }
