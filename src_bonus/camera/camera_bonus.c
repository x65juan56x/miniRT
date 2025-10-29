#include <math.h>
#include "../../include_bonus/camera_bonus.h"
#include "../../include/math_utils.h"
#include "../../include_bonus/scene_bonus.h"

void	camera_build_frame(const t_camera *cam, int width, int height,
			t_cam_frame *out)
{
	t_vec3	up_world;
	float	aspect;
	float	half_w;
	float	half_h;
	t_vec3	center;

	out->origin = cam->pos;
	out->forward = cam->dir;
	up_world = v3(0.0f, 1.0f, 0.0f);
	if (fabsf(v3_dot(out->forward, up_world)) > 0.999f)
		up_world = v3(0.0f, 0.0f, 1.0f);
	out->right = v3_norm(v3_cross(out->forward, up_world));
	out->up = v3_cross(out->right, out->forward);
	aspect = (float)width / (float)height;
	half_w = tanf(deg2rad(cam->fov_deg) * 0.5f) * cam->focal;
	half_h = half_w / aspect;
	out->horizontal = v3_mul(out->right, 2.0f * half_w);
	out->vertical = v3_mul(out->up, 2.0f * half_h);
	center = v3_add(out->origin, v3_mul(out->forward, cam->focal));
	out->lower_left = v3_sub(v3_sub(center, v3_mul(out->horizontal, 0.5f)),
			v3_mul(out->vertical, 0.5f));
}
