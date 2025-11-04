/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_lines_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:29:55 by jmondon           #+#    #+#             */
/*   Updated: 2025/11/04 16:29:56 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../../include_bonus/parser_internal_bonus.h"

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
