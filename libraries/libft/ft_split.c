/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 21:51:30 by jmondon           #+#    #+#             */
/*   Updated: 2025/06/27 18:58:16 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_wrdctr(char const *s, char c)
{
	size_t	i;
	size_t	w;

	i = 0;
	w = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			w++;
			while (s[i] != c && s[i])
				i++;
			if (!s[i])
				return (w);
		}
		else
			i++;
	}
	return (w);
}

char	**ft_split(char const *s, char c)
{
	size_t	i;
	size_t	j;
	size_t	k;
	char	**arr;

	arr = (char **)malloc((ft_wrdctr(s, c) + 1) * sizeof(char *));
	if (!arr || !s)
		return (NULL);
	i = -1;
	k = 0;
	while (s[++i])
	{
		if (s[i] != c)
		{
			j = i;
			while (s[i] != c && s[i])
				i++;
			arr[k] = ft_substr(s, j, (i - j));
			if (!arr[k++])
				return (ft_freearr(arr));
			i--;
		}
	}
	arr[k] = 0;
	return (arr);
}
