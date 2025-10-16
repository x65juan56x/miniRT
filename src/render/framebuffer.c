#include "../../include/render.h"

void	upload_framebuffer(mlx_image_t *image, const uint32_t *fb)
{
	// Fast path: write RGBA bytes directly into the image buffer
	uint8_t *dst = image->pixels;
	uint32_t w = image->width;
	uint32_t h = image->height;
	uint32_t count = w * h;
	for (uint32_t i = 0; i < count; ++i)
	{
		uint32_t c = fb[i];
		// Our colors are packed as (r<<24)|(g<<16)|(b<<8)|a
		dst[i * 4 + 0] = (uint8_t)((c >> 24) & 0xFF); // R
		dst[i * 4 + 1] = (uint8_t)((c >> 16) & 0xFF); // G
		dst[i * 4 + 2] = (uint8_t)((c >> 8) & 0xFF);  // B
		dst[i * 4 + 3] = (uint8_t)(c & 0xFF);         // A
	}
}