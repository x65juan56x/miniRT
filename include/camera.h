/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:02:44 by anagarri          #+#    #+#             */
/*   Updated: 2025/11/04 16:02:46 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_H
# define CAMERA_H

# include "minirt.h"
# include "scene.h"

typedef struct s_cam_frame
{
	t_vec3	origin;
	t_vec3	forward;
	t_vec3	right;
	t_vec3	up;
	t_vec3	horizontal;
	t_vec3	vertical;
	t_vec3	lower_left;
}	t_cam_frame;

void	camera_build_frame(const t_camera *cam, int width, int height,
			t_cam_frame *out);

#endif
