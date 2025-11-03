#include <stdint.h>
#include "../../include/minirt.h"

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
/*
* Purpose: Copy the rendered framebuffer to the MLX image for display.
* Logic: Iterate through all pixels; byte-swap each RGBA value before writing.
* Notes: ft_swap_u32 converts endianness (RGBA ↔ ABGR) for MLX compatibility.
*        Null-checks prevent crashes if image or framebuffer is invalid.
* Use: Called after rendering to update the on-screen image; count is
	total pixels.
*/
