/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 16:19:23 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 16:19:27 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_tolower(int c)
{
	if ((64 < c) && (c < 91))
		c = c + 32;
	return (c);
}
// #include <stdio.h>
// #include <ctype.h>

// int main()
// {
// 	char c = 'A';
// 	printf("ft_tolower vs. tolower\n");
// 	printf("Character: %c\n", c);
// 	printf("Original tolower: %c\n", tolower(c));
// 	printf("libft ft_tolower: %c\n", ft_tolower(c));
// 	return (0);
// }
