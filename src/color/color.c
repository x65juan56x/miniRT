#include "../../include/color.h"
#include "../../include/vec3.h"
#include "../../include/math_utils.h"

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
	int	r;
	int	g;
	int	b;

	r = (int)(clampf(rgb.x, 0.0f, 1.0f) * 255.0f + 0.5f);
	g = (int)(clampf(rgb.y, 0.0f, 1.0f) * 255.0f + 0.5f);
	b = (int)(clampf(rgb.z, 0.0f, 1.0f) * 255.0f + 0.5f);
	return (rgba_u32(clamp_u8i(r), clamp_u8i(g), clamp_u8i(b), 255));
}
/*
Purpose: Convert a color in vector format (float [0,1]) to an
	RGBA integer (uint32_t).
Logic:
  - Clamp each component (x, y, z) to the range [0,1] using clampf.
  - Multiply by 255 and add 0.5 to round before casting to int.
  - clamp_u8i ensures the final value is in [0,255] (uint8_t).
  - rgba_u32 packs the components into a uint32_t (RGBA order).
Variables:
  - rgb: color as a vector (float x, y, z).
  - r, g, b: integer color components.
The +0.5f in the formula:
Used to correctly round the floating-point value to the nearest integer.
Without +0.5f, the cast to (int) simply truncates (cuts decimals downward),
which can bias colors toward lower values.
For example:
  If the result is 127.8, with +0.5f → 128.3 → (int) → 128 (correct).
  If the result is 127.2, with +0.5f → 127.7 → (int) → 127.
This is a standard technique for converting float to int with rounding instead
of truncation.
*/
