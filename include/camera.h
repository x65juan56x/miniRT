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
