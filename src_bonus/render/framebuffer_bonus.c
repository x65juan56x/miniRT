/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   framebuffer_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:28:31 by jmondon           #+#    #+#             */
/*   Updated: 2025/11/04 16:28:31 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include "../../include_bonus/minirt_bonus.h"

void	upload_framebuffer(mlx_image_t *image, const uint32_t *fb)
{
	uint32_t	*dst;
	size_t		count;
	size_t		i;

	if (!image || !fb || !image->pixels)
		return ;
	dst = (uint32_t *)image->pixels;
	count = (size_t)image->width * (size_t)image->height;
	i = 0;
	while (i < count)
	{
		dst[i] = ft_swap_u32(fb[i]);
		i++;
	}
}
