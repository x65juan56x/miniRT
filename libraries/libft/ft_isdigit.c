/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 16:29:25 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 16:29:28 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isdigit(int c)
{
	if (!((48 <= c) && (c <= 57)))
		return (0);
	return (1);
}
// #include <stdio.h>
// #include <ctype.h>

// int main()
// {
// 	char c = '5';

// 	printf("ft_isdigit vs. isdigit\n");
// 	printf("Character: %c\n", c);
// 	printf("Original isdigit: %d\n", isdigit(c));
// 	printf("libft ft_isdigit: %d\n", ft_isdigit(c));
// 	return (0);
// }
