/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 19:45:28 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/14 14:22:29 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	size_t	i;
	char	chr;
	char	*str;

	chr = (unsigned char)c;
	str = (char *)s;
	i = ft_strlen(s);
	while (i > 0)
	{
		if (s[i] == chr)
			return (str + i);
		i--;
	}
	if (s[i] == chr)
		return (str);
	return (NULL);
}
/*
Line 21:
    Assign "chr" the value of "c" cast as (unsigned char) to avoid issues
    when "c" has values outside the ASCII character range (256 < c < 0).
Line 22:
    I could avoid using the "str" variable by directly returning
    ((char *)s + i); on line 27, but casting in each loop iteration is
	inefficient.
Lines 30 and 31:
    By assigning the size_t type to the index "i", I cannot include the case
    where the index is 0 within the while loop since the "i >= 0" statement
    would always be true. Therefore, these lines handle the case where the
    first character is null.
*/

// #include <stdio.h>
// #include <string.h>

// int main()
// {
// 	const char *str = "Hello, world!";
// 	int c = 'o';

// 	printf("ft_strrchr vs. strrchr\n");
// 	printf("String: %s\n", str);
// 	printf("Character: %c\n", c);
// 	printf("Original strrchr: %s\n", strrchr(str, c));
// 	printf("libft ft_strrchr: %s\n", ft_strrchr(str, c));
// 	return (0);
// }
