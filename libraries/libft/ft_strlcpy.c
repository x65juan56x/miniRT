/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 16:07:25 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 16:07:27 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	i;

	i = 0;
	if (size == 0)
	{
		while (src[i])
			i++;
		return (i);
	}
	while (src[i] != 0 && i < size - 1)
	{
		dest[i] = src[i];
		i++;
	}
	if (i < size)
		dest[i] = '\0';
	while (src[i] != 0)
		i++;
	return (i);
}
// #include <stdio.h>
// #include <string.h>

// int main()
// {
// 	const char *src = "Hello, world!";
// 	char dest1[20];
// 	char dest2[20];
// 	size_t size = 5;

// 	printf("ft_strlcpy vs. strlcpy\n");
// 	printf("Source string: '%s'\n", src);
// 	printf("Size: %zu\n", size);
// 	printf("strlcpy: %zu, Res: '%s'\n", strlcpy(dest1, src, size), dest1);
// 	printf("ft_strlcpy: %zu, Res: '%s'\n", ft_strlcpy(dest2, src, size), dest2);
// 	return (0);
// }
