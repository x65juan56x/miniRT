#ifndef COLOR_H
# define COLOR_H

# include <stdint.h>
# include "vec3.h"

uint32_t	rgba_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

uint8_t		clamp_u8i(int v);

uint32_t	vec3_to_rgba(t_vec3 rgb);

#endif
