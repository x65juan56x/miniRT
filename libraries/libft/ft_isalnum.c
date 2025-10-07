/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 16:26:39 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 16:26:45 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalnum(int c)
{
	if (!((('a' <= c) && (c <= 'z'))
			|| (('A' <= c) && (c <= 'Z'))
			|| ((48 <= c) && (c <= 57))))
		return (0);
	return (1);
}
// #include <stdio.h>
// #include <ctype.h>

// int main() {
// 	char c = 'A';

// 	printf("ft_isalnum vs. isalnum\n");
// 	printf("Character: %c\n", c);
// 	printf("Original isalnum: %d\n", isalnum(c));
// 	printf("libft ft_isalnum: %d\n", ft_isalnum(c));
// 	return 0;
// }
