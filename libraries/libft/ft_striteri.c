/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 21:20:35 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/10 21:20:37 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char*))
{
	unsigned int	i;

	i = 0;
	while (s[i])
	{
		f(i, &s[i]);
		i++;
	}
}
// #include <stdio.h>

// void example_func(unsigned int i, char *c)
// {
// 	if (*c >= 'a' && *c <= 'z')
// 		*c = *c - 32;
// }

// int main()
// {
// 	char s[] = "Hello, world!";

// 	printf("ft_striteri\n");
// 	printf("Original string: '%s'\n", s);
// 	ft_striteri(s, example_func);
// 	printf("Resulting string: '%s'\n", s);
// 	return (0);
// }
