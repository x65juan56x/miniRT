/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 20:30:38 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 20:30:42 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *str, const char *to_find, size_t n)
{
	size_t	i;
	size_t	j;
	char	*s;

	s = (char *)str;
	i = 0;
	if (to_find[0] == 0)
		return (s);
	while (str[i] != '\0' && i < n)
	{
		j = 0;
		while (str[i + j] != '\0' && (str[i + j] == to_find[j] && i + j < n))
		{
			if (to_find[j + 1] == '\0')
				return (&s[i]);
			j++;
		}
		i++;
	}
	return (NULL);
}
// #include <stdio.h>
// #include <string.h>

// int main()
// {
// 	const char *str = "Hello, world!";
// 	const char *to_find = "world";
// 	size_t n = 13;

// 	printf("ft_strnstr vs. strnstr\n");
// 	printf("String: '%s'\n", str);
// 	printf("Substring to find: '%s'\n", to_find);
// 	printf("Length to search: %zu\n", n);
// 	printf("Original strnstr: %s\n", strnstr(str, to_find, n));
// 	printf("libft ft_strnstr: %s\n", ft_strnstr(str, to_find, n));
// 	return (0);
// }
