/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 17:01:03 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 17:01:06 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isascii(int c)
{
	if (!((0 <= c) && (c <= 127)))
		return (0);
	return (1);
}
// #include <stdio.h>
// #include <ctype.h>

// int main()
// {
// 	int c = 65;

// 	printf("Testing ft_isascii vs. isascii\n");
// 	printf("Character: %d\n", c);
// 	printf("Original isascii: %d\n", isascii(c));
// 	printf("libft ft_isascii: %d\n", ft_isascii(c));
// 	return (0);
// }
