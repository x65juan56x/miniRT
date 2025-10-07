/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 19:58:49 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/14 15:09:00 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_ifsmaj(void *dest, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*s;
	unsigned char	*d;

	s = (unsigned char *)src;
	d = (unsigned char *)dest;
	i = 0;
	while (i < n)
	{
		d[i] = s[i];
		i++;
	}
}

void	ft_ifsmin(void *dest, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*s;
	unsigned char	*d;

	s = (unsigned char *)src;
	d = (unsigned char *)dest;
	i = n - 1;
	while (i > 0)
	{
		d[i] = s[i];
		i--;
	}
	d[i] = s[i];
}

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	if (!src && !dest)
		return (NULL);
	if (n == 0)
		return (dest);
	if (dest < src)
		ft_ifsmaj(dest, src, n);
	else
		ft_ifsmin(dest, src, n);
	return (dest);
}
/*
The pointers I receive as parameters point to a memory space where the data
type is not defined (void), so I cannot directly use these pointers to copy
from one address to another. Therefore, I declare the variables d and s,
assigning them the values of dest and src, respectively, but casting them as
"unsigned char". Now I have two pointers pointing to the same places as
dest and src, but with a defined data type.
Additionally, I use "unsigned char" for these pointers to avoid out-of-range
value issues.
Line 50:
	On line 50, I use this condition to handle the case where either
	parameter is null, which would mean no memory address is assigned to
	that parameter.
	The original function uses && instead of ||; I'm not sure why.
Line 52:
	Explicitly handle the case where n = 0, as it is not covered in ft_ifsmaj
	or ft_ifsmin.
I split the function into 3 functions to avoid exceeding the number of lines
without losing clarity in the code and to use the size_t type for i, which
requires separately handling the case where n = 0.
*In the ft_ifsmaj function (if src's memory address is greater than dest's
memory address), copy the string upward since dest, being less than src and
advancing at the same time, will never overwrite src before it has been
copied to dest.
*In the ft_ifsmin function, iterate backward to avoid overwriting information
that has not yet been copied.
*/

// #include <stdio.h>
// #include <string.h>

// int main()
// {
// 	char src[] = "Hello, world!";
// 	char dest1[50];
// 	char dest2[50];
// 	size_t n = 7;

// 	printf("ft_memmove vs. memmove\n");
// 	printf("Source string: '%s'\n", src);
// 	memmove(dest1, src, n);
// 	ft_memmove(dest2, src, n);
// 	printf("Original memmove: '%s'\n", dest1);
// 	printf("libft ft_memmove: '%s'\n", dest2);
// 	return (0);
// }
