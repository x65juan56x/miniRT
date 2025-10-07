/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isprint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 20:21:29 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 20:21:36 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isprint(int c)
{
	if (!((32 <= c) && (c <= 126)))
		return (0);
	return (1);
}
// #include <stdio.h>
// #include <ctype.h>

// int main()
// {
// 	char c = 'A';

// 	printf("ft_isprint vs. isprint\n");
// 	printf("Character: %c\n", c);
// 	printf("Original isprint: %d\n", isprint(c));
// 	printf("libft ft_isprint: %d\n", ft_isprint(c));
// 	return (0);
// }
