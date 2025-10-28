#include <stdint.h>
#include "../../include/minirt_bonus.h"

void	upload_framebuffer(mlx_image_t *image, const uint32_t *fb)
{
	int y;
	int x;

	if (!image || !fb)
		return;
	y = 0;
	while ((uint32_t)y < image->height)
	{
		x = 0;
		while ((uint32_t)x < image->width)
		{
			mlx_put_pixel(image, x, y, fb[(size_t)y * image->width + (size_t)x]);
			x++;
		}
		y++;
	}
}
