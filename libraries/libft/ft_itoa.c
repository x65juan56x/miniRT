/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 19:38:28 by jmondon           #+#    #+#             */
/*   Updated: 2025/07/21 15:55:18 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_alng(long n)
{
	int	l;

	l = 0;
	if (n <= 0)
		l = 1;
	while (n)
	{
		n /= 10;
		l++;
	}
	return (l);
}

char	*ft_itoa(int n)
{
	long	i;
	int		l;
	char	*a;

	i = (long)n;
	l = ft_alng(i);
	a = (char *)malloc((l + 1) * sizeof(char));
	if (!a)
		return (NULL);
	a[l] = '\0';
	if (i == 0)
		a[0] = '0';
	if (i == 0)
		return (a);
	if (i < 0)
		a[0] = '-';
	if (i < 0)
		i = -i;
	while (i > 0)
	{
		a[--l] = (i % 10) + '0';
		i /= 10;
	}
	return (a);
}

/*
ft_alng:
	Es una función que uso para calcular de antemano cuanto dígitos va a tener
	la cadena resultante, para no hacer asignaciones de memoria innecesarias.
	Linea 20: Compruebo si el número es negativo y si es así ya le asigno un
	lugar para el signo.
	Línea 22: acá voy dividiendo el número a analizar por diez en cada
	iteración, para contar cuantas veces es divisible por diez, lo cual
	equivale a la cantidad de dígitos que tendrá la cadena final.
*/

/*
char	*ft_itoa(int n)
{
	long	i;
	char	*a;

	if (n == -2147483648)
		return (ft_strdup("-2147483648"));
	i = (long)n;
	a = (char *)ft_calloc(sizeof(char), 2);
	if (a == NULL)
		return (NULL);
	if (i < 0)
	{
		a = ft_strjoin("-", ft_itoa(i * -1));
	}
	if (i > 9)
		a = ft_strjoin(ft_itoa(i / 10), ft_itoa(i % 10));
	if (i < 10 && i >= 0)
		a[0] = i + '0';
	return (a);
}
*/
// #include <stdio.h>

// int main()
// {
// 	int num = -12345;
// 	char *str;

// 	printf("ft_itoa\n");
// 	printf("Number: %d\n", num);
// 	str = ft_itoa(num);
// 	if (str != NULL)
// 	{
// 		printf("Resulting string: '%s'\n", str);
// 		free(str);
// 	}
// 	else
// 		printf("ft_itoa returned NULL\n");
// 	return (0);
// }
