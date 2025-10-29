#ifndef MINIRT_BONUS_H
# define MINIRT_BONUS_H

# include <math.h>
# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <limits.h>
# include "../libraries/MLX42/include/MLX42/MLX42.h"
# include "../libraries/libft/libft.h"
# include "../include/color.h"
# include "../include/vec3.h"
# include "../include/ray.h"
# include "../include/math_utils.h"

/*
 * Include common core project headers that do not create circular dependencies.
 * Avoid including render_bonus.h or camera_bonus.h here.
 */
# include "scene_bonus.h"
# include "hit_bonus.h"
# include "shading_bonus.h"
# include "parser_bonus.h"
# include "material_bonus.h"
# include "bump_bonus.h"

# define WIN_W 1920
# define WIN_H 1080

// ε para evitar self-intersections (lo vamos a usar más adelante en ray tracing)
# define EPSILON 1e-4f

#endif
