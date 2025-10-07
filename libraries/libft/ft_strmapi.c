/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 20:55:53 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/10 20:55:56 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char			*str;
	unsigned int	i;

	str = ft_calloc(ft_strlen(s) + 1, sizeof(char));
	if (!str || !s)
		return (NULL);
	i = 0;
	while (s[i])
	{
		str[i] = f(i, s[i]);
		i++;
	}
	return (str);
}
// #include <stdio.h>

// char example_func(unsigned int i, char c) {
// 	if (c >= 'a' && c <= 'z')
// 		return c - 32;
// 	return c;
// }

// int main()
// {
// 	const char *s = "Hello, world!";
// 	char *result;

// 	printf("ft_strmapi\n");
// 	printf("Original string: '%s'\n", s);
// 	result = ft_strmapi(s, example_func);
// 	if (result != NULL)
// 	{
// 		printf("Resulting string: '%s'\n", result);
// 		free(result);
// 	}
// 	else
// 		printf("ft_strmapi returned NULL\n");
// 	return (0);
// }
