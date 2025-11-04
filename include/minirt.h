/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minirt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:03:28 by anagarri          #+#    #+#             */
/*   Updated: 2025/11/04 16:03:29 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINIRT_H
# define MINIRT_H

# include <math.h>
# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <limits.h>
# include "../libraries/MLX42/include/MLX42/MLX42.h"
# include "../libraries/libft/libft.h"
# include "color.h"
# include "vec3.h"
# include "ray.h"
# include "math_utils.h"

# define WIN_W 1920
# define WIN_H 1080
// ε to avoid self-intersections (we’ll use it later in ray tracing)
# define EPSILON 1e-4f

#endif
