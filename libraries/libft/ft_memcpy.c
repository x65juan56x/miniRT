/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 21:57:28 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/14 14:05:46 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*d;
	unsigned char	*s;

	d = (unsigned char *)dest;
	s = (unsigned char *)src;
	if (!src && !dest)
		return (NULL);
	while (n > 0)
	{
		d[n - 1] = s[n - 1];
		n--;
	}
	return (dest);
}
/*
    The pointers received as parameters point to memory spaces where the
    data type is not defined (void), so I cannot directly use these pointers
    to copy from one address to another. I declare the variables d and s,
    assigning them the values of dest and src respectively, casting them
    as "unsigned char". Now I have two pointers pointing to the same places
    as dest and src, but with a defined data type.
    Additionally, I use "unsigned char" for these pointers to avoid issues
    with out-of-range values.
    Line 22:
        On line 22, this condition ensures that if either parameter is null,
        it does not have an assigned memory address.
        The original function uses && instead of ||, which I don't understand.
*/

// #include <stdio.h>
// #include <string.h>

// int main()
// {
// 	char src[] = "Hello, world!";
// 	char dest1[50];
// 	char dest2[50];
// 	size_t n = strlen(src) + 1;

// 	printf("ft_memcpy vs. memcpy\n");
// 	printf("Source string: '%s'\n", src);
// 	memcpy(dest1, src, n);
// 	ft_memcpy(dest2, src, n);
// 	printf("Original memcpy: '%s'\n", dest1);
// 	printf("libft ft_memcpy: '%s'\n", dest2);
// 	return (0);
// }
