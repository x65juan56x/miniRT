/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   math_utils.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:03:20 by anagarri          #+#    #+#             */
/*   Updated: 2025/11/04 16:03:21 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATH_UTILS_H
# define MATH_UTILS_H

# ifndef M_PI
#  define M_PI 3.1415926535897932385
# endif

float	deg2rad(float d);
float	clampf(float value, float min, float max);

#endif
