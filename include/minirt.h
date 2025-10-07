#ifndef MINIRT_H
# define MINIRT_H

# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include "../libraries/MLX42/include/MLX42/MLX42.h"
# include "color.h"
# include "vec3.h"
# include "ray.h"

# define WIN_W 1920
# define WIN_H 1080

// ε para evitar self-intersections (lo vamos a usar más adelante en ray tracing)
# define EPSILON 1e-4f

#endif
