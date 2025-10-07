/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_getpid.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:13:28 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/04 13:13:30 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <fcntl.h>

static int	read_digit(int fd)
{
	char	c;

	if (read(fd, &c, 1) != 1)
		return (-1);
	return (c);
}

int	ft_getpid(void)
{
	int	fd;
	int	pid;
	int	digit;
	int	count;

	pid = 0;
	count = 0;
	fd = open("/proc/self/stat", O_RDONLY);
	if (fd < 0)
		return (-1);
	while (1)
	{
		digit = read_digit(fd);
		if (digit == -1 || !ft_isdigit(digit))
			break ;
		pid = pid * 10 + (digit - '0');
		count++;
	}
	close(fd);
	if (count > 0)
		return (pid);
	else
		return (-1);
}
