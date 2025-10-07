/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 18:54:09 by jmondon           #+#    #+#             */
/*   Updated: 2025/06/27 18:59:20 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	size_t	i;
	char	chr;
	char	*str;

	chr = (unsigned char)c;
	str = (char *)s;
	i = 0;
	while (s[i])
	{
		if (s[i] == chr)
			return (str + i);
		i++;
	}
	if (chr == s[i])
		return (str + i);
	return (NULL);
}
