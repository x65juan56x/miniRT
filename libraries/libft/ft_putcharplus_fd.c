/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putcharplus_fd.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 13:14:05 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/04 13:24:57 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putcharplus_fd(int fd, char c)
{
	ssize_t	bytes_written;

	while (1)
	{
		bytes_written = write(fd, &c, 1);
		if (bytes_written == 1)
			return (0);
		return (-1);
	}
}
