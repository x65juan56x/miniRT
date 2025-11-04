/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_lines.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:28:06 by anagarri          #+#    #+#             */
/*   Updated: 2025/11/04 16:28:07 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../../include/parser_internal.h"

void	free_lines(char **lines, size_t count)
{
	size_t	index;

	if (!lines)
		return ;
	index = 0;
	while (index < count)
	{
		free(lines[index]);
		index++;
	}
	free(lines);
}
/*
* Purpose: Free the array of strings produced by `read_file_lines`.
* Notes: Iterates over the recorded count before releasing the pointer array.
*/
