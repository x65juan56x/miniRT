/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 16:00:06 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 16:00:12 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	des_len;
	size_t	src_len;

	i = 0;
	des_len = ft_strlen(dest);
	src_len = ft_strlen(src);
	if (size == 0)
		return (src_len);
	if (size <= des_len)
		return (size + src_len);
	while (src[i] && (i < (size - des_len -1)))
	{
		dest[des_len + i] = src[i];
		i++;
	}
	dest[des_len + i] = '\0';
	return (des_len + src_len);
}
// #include <stdio.h>
// #include <string.h>

// int main()
// {
// 	char dest1[30] = "Hello, ";
// 	const char *src = "world!";
// 	char dest2[30] = "Hello, ";
// 	size_t size = 20;

// 	printf("ft_strlcat vs. strlcat\n");
// 	printf("Destination before: '%s'\n", dest1);
// 	printf("Source string: '%s'\n", src);
// 	printf("Size: %zu\n", size);
// 	printf("strlcat: %zu, Res: '%s'\n", strlcat(dest1, src, size), dest1);
// 	printf("ft_strlcat: %zu, Res: '%s'\n", ft_strlcat(dest2, src, size), dest2);
// 	return (0);
// }
