#include "../../include/render.h"

void	upload_framebuffer(mlx_image_t *image, const uint32_t *fb)
{
	uint8_t		*dst;
	uint32_t	count;
	uint32_t	i;
	uint32_t	c;

	dst = image->pixels;
	count = image->width * image->height;
	i = 0;
	while (i < count)
	{
		c = fb[i];
		dst[i * 4 + 0] = (uint8_t)((c >> 24) & 0xFF); // R
		dst[i * 4 + 1] = (uint8_t)((c >> 16) & 0xFF); // G
		dst[i * 4 + 2] = (uint8_t)((c >> 8) & 0xFF);  // B
		dst[i * 4 + 3] = (uint8_t)(c & 0xFF);         // A
		i++;
	}
}
