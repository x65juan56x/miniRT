#include <float.h>
#include "../../include/render.h"
#include "../../include/camera.h"
#include "../../include/hit.h"
#include "../../include/shading.h"

static t_vec3	trace_pixel(const t_scene *scene, t_ray r, int show_normals)
{
	t_hit	hit;

	scene_hit(scene, r, FLT_MAX, &hit);
	if (show_normals && hit.ok)
		return (v3_mul(v3_add(hit.n, v3(1.0f, 1.0f, 1.0f)), 0.5f));
	return (shade_lambert(scene, &hit));
}

void	render_scene(uint32_t *fb, int width, int height, const t_scene *scene, int show_normals)
{
	t_cam_frame	frame;
	int			x;
	int			y;
	float		u;
	float		v;
	t_vec3		sample;
	t_vec3		dir;

	camera_build_frame(&scene->camera, width, height, &frame);
	y = 0;
	while (y < height)
	{
		x = 0;
		while (x < width)
		{
			u = ((float)x + 0.5f) / (float)width;
			v = 1.0f - (((float)y + 0.5f) / (float)height);
			sample = v3_add(frame.lower_left, v3_add(v3_mul(frame.horizontal, u), v3_mul(frame.vertical, v)));
			dir = v3_norm(v3_sub(sample, frame.origin));
			dir = v3_norm(v3_sub(sample, frame.origin));
			fb[y * width + x] = vec3_to_rgba(trace_pixel(scene, ray(frame.origin, dir), show_normals));
			x++;
		}
		y++;
	}
}
