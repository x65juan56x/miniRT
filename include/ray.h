/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:04:00 by anagarri          #+#    #+#             */
/*   Updated: 2025/11/04 16:04:01 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAY_H
# define RAY_H

# include "vec3.h"

typedef struct s_ray
{
	t_vec3	orig;
	t_vec3	dir;
}	t_ray;

t_ray	ray(t_vec3 o, t_vec3 d);
t_vec3	ray_at(t_ray r, float t);

#endif
