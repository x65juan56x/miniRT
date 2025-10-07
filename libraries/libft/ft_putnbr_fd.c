/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 12:31:27 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/05 12:31:31 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	if (n == -2147483648)
	{
		ft_putchar_fd('-', fd);
		ft_putchar_fd('2', fd);
		n = 147483648;
	}
	if (n < 0)
	{
		ft_putchar_fd('-', fd);
		n *= -1;
	}
	if (n >= 10)
	{
		ft_putnbr_fd(n / 10, fd);
	}
	ft_putchar_fd(n % 10 + '0', fd);
}
// #include <fcntl.h>
// #include <unistd.h>

// int main()
// {
// 	int num = -12345;
// 	int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

// 	if (fd < 0)
// 	{
// 		perror("Error opening file");
// 		return (1);
// 	}
// 	printf("ft_putnbr_fd\n");
// 	printf("Number to write: %d\n", num);
// 	ft_putnbr_fd(num, fd);
// 	close(fd);
// 	printf("Number written to 'output.txt'\n");
// 	return (0);
// }
