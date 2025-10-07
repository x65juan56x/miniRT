/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 16:13:54 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/02 16:13:57 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_atoi(const char *str)
{
	unsigned int	i;
	long			sign;
	long			value;

	i = 0;
	while ((str[i] == ' ') || (str[i] <= 13 && str[i] >= 9))
		i++;
	sign = 1;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i +1] == '-' || str[i + 1] == '+')
			return (0);
		if (str[i] == '-')
			sign *= -1;
		i++;
	}
	value = 0;
	while (str[i] >= 48 && str[i] <= 57 && str[i] != 0)
	{
		value = (value * 10) + str[i] - '0';
		i++;
	}
	return ((int)(value * sign));
}
// #include <stdio.h>
// #include <stdlib.h>

// int main()
// {
// 	const char *str = "   -12345";

// 	printf("ft_atoi vs. atoi\n");
// 	printf("String: '%s'\n", str);
// 	printf("Original atoi: %d\n", atoi(str));
// 	printf("libft ft_atoi: %d\n", ft_atoi(str));

// 	return (0);
// }
