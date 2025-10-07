/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_toupper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 16:17:18 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 16:17:22 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_toupper(int c)
{
	if ((96 < c) && (c < 123))
		c = c - 32;
	return (c);
}
// #include <stdio.h>
// #include <ctype.h>

// int main()
// {
// 	char c = 'a';

// 	printf("ft_toupper vs. toupper\n");
// 	printf("Character: %c\n", c);
// 	printf("Original toupper: %c\n", toupper(c));
// 	printf("libft ft_toupper: %c\n", ft_toupper(c));
// 	return (0);
// }
