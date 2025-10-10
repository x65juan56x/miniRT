#include "../../include/color.h"
#include "../../include/vec3.h"
#include "../../include/aux_math.h"

inline uint32_t	rgba_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (((uint32_t)r << 24) | ((uint32_t)g << 16)
		| ((uint32_t)b << 8) | (uint32_t)a);
}

inline uint8_t	clamp_u8i(int v)
{
	if (v < 0)
		return (0);
	if (v > 255)
		return (255);
	return ((uint8_t)v);
}

uint32_t	vec3_to_rgba(t_vec3 rgb)
{
	int	 r;
	int	 g;
	int	 b;

	r = (int)(clampf(rgb.x, 0.0f, 1.0f) * 255.0f + 0.5f);
	g = (int)(clampf(rgb.y, 0.0f, 1.0f) * 255.0f + 0.5f);
	b = (int)(clampf(rgb.z, 0.0f, 1.0f) * 255.0f + 0.5f);
	return (rgba_u32(clamp_u8i(r), clamp_u8i(g), clamp_u8i(b), 255));
}
