/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_error.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:30:41 by jmondon           #+#    #+#             */
/*   Updated: 2025/06/27 19:28:37 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

void	ft_perror_exit(const char *prefix, int exit_code)
{
	if (prefix)
		perror(prefix);
	else
		perror("minishell");
	exit(exit_code);
}

void	ft_cmd_not_found_exit(const char *cmd_name)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (cmd_name && *cmd_name)
	{
		ft_putstr_fd((char *)cmd_name, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	ft_putstr_fd("command not found\n", STDERR_FILENO);
	exit(127);
}

void	ft_execve_error_exit(const char *cmd_name)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (cmd_name && *cmd_name)
	{
		ft_putstr_fd((char *)cmd_name, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	ft_putstr_fd(strerror(errno), STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
	exit(126);
}

void	ft_error(void)
{
	perror("minishell");
	exit(EXIT_FAILURE);
}
