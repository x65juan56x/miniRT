/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 12:34:44 by jmondon           #+#    #+#             */
/*   Updated: 2024/12/05 12:34:48 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}
// #include <fcntl.h>
// #include <unistd.h>

// int main()
// {
// 	char c = 'A';
// 	int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

// 	if (fd < 0)
// 	{
// 		perror("Error opening file");
// 		return (1);
// 	}
// 	printf("ft_putchar_fd\n");
// 	printf("Character to write: '%c'\n", c);
// 	ft_putchar_fd(c, fd);
// 	close(fd);
// 	printf("Character written to 'output.txt'\n");
// 	return (0);
// }
