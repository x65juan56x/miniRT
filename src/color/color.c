#include "../../include/color.h"

inline uint32_t	rgba_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r);
}

inline uint8_t	clamp_u8i(int v)
{
	if (v < 0)
		return (0);
	if (v > 255)
		return (255);
	return ((uint8_t)v);
}
