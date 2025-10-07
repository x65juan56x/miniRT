#include <stdlib.h>
#include <math.h>
#include "../../libraries/libft/libft.h"
#include "../../include/parser_internal.h"

bool	parse_vec3(const char *s, t_vec3 *out)
{
	char	**tokens;
	size_t	index;
	float	values[3];

	tokens = ft_split(s, ',');
	if (!tokens)
		return (false);
	if (!tokens[0] || !tokens[1] || !tokens[2] || tokens[3])
		return (free_tokens(tokens), false);
	index = 0;
	while (index < 3)
	{
		if (!parse_float(tokens[index], &values[index]))
			return (free_tokens(tokens), false);
		index++;
	}
	out->x = values[0];
	out->y = values[1];
	out->z = values[2];
	return (free_tokens(tokens), true);
}
/*
* Purpose: Parse a "x,y,z" triple into a `t_vec3` with floating components.
* Success: Fills `out` when exactly three valid floats are provided.
* Failure: Returns false if allocation fails or the string is malformed.
*/

bool	parse_color_255(const char *s, t_vec3 *out)
{
	char	**tokens;
	size_t	index;
	int		values[3];

	tokens = ft_split(s, ',');
	if (!tokens)
		return (false);
	if (!tokens[0] || !tokens[1] || !tokens[2] || tokens[3])
		return (free_tokens(tokens), false);
	index = 0;
	while (index < 3)
	{
		if (!parse_int_in_range(tokens[index], 0, 255, &values[index]))
			return (free_tokens(tokens), false);
		index++;
	}
	out->x = (float)values[0] / 255.0f;
	out->y = (float)values[1] / 255.0f;
	out->z = (float)values[2] / 255.0f;
	return (free_tokens(tokens), true);
}
/*
* Purpose: Decode an RGB triplet in 0..255 and map it to normalized floats.
* Success: Populates `out` with values scaled to the [0,1] range.
* Failure: Returns false on allocation errors or invalid integer components.
*/

bool	vec3_is_normalized(t_vec3 v)
{
	float	len2;

	len2 = v3_len2(v);
	return (fabsf(len2 - 1.0f) <= 2e-3f);
}
/*
* Purpose: Check whether a vector length is approximately one.
* Behavior: Uses squared length tolerance to sidestep costly square roots.
* Application: Validates orientation vectors supplied in the scene file.
*/

bool	vec3_components_in_range(t_vec3 v, float minv, float maxv)
{
	if (v.x < minv || v.x > maxv)
		return (false);
	if (v.y < minv || v.y > maxv)
		return (false);
	if (v.z < minv || v.z > maxv)
		return (false);
	return (true);
}
/*
* Purpose: Ensure each component of `v` sits inside the inclusive interval.
* Use case: Clamp parser inputs before using them as directions or colors.
*/
