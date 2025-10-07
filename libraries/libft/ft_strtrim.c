/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 23:15:44 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/04 23:15:50 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	i;

	if (!s1 || !set)
		return (NULL);
	while (*s1 && ft_strchr(set, *s1))
		s1++;
	i = ft_strlen(s1);
	while (i && ft_strchr(set, s1[i]))
		i--;
	return (ft_substr(s1, 0, i + 1));
}
// #include <stdio.h>

// int main()
// {
// 	const char *s1 = "  Hello, world!  ";
// 	const char *set = " ";
// 	char *trimmed_str;

// 	printf("ft_strtrim\n");
// 	printf("Original string: '%s'\n", s1);
// 	printf("Characters to trim: '%s'\n", set);
// 	trimmed_str = ft_strtrim(s1, set);
// 	if (trimmed_str != NULL)
// 	{
// 		printf("Resulting trimmed string: '%s'\n", trimmed_str);
// 		free(trimmed_str);
// 	}
// 	else
// 		printf("ft_strtrim returned NULL\n");
// 	return (0);
// }
