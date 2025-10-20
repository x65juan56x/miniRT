#include <float.h>
#include "../../include/render.h"
#include "../../include/camera.h"
#include "../../include/app.h"
#include "../../include/shading.h"

static t_vec3	trace_pixel(const t_scene *scene, t_ray r, int show_normals)
{
	t_hit	hit;

	scene_hit(scene, r, FLT_MAX, &hit);
	if (show_normals && hit.ok)
		return (v3_mul(v3_add(hit.n, v3(1.0f, 1.0f, 1.0f)), 0.5f));
	return (shade_lambert(scene, &hit));
}

void	render_scene(t_app *app)
{
	int				x;
	int				y;
	t_render_aux	vars;

	camera_build_frame(&app->scene.camera, app->image->width,
		app->image->height, &vars.frame);
	y = -1;
	while ((uint32_t)++y < app->image->height)
	{
		x = -1;
		while ((uint32_t)++x < app->image->width)
		{
			vars.u = ((float)x + 0.5f) / (float)app->image->width;
			vars.v = 1.0f - (((float)y + 0.5f) / (float)app->image->height);
			vars.sample = v3_add(vars.frame.lower_left,
					v3_add(v3_mul(vars.frame.horizontal, vars.u),
						v3_mul(vars.frame.vertical, vars.v)));
			vars.dir = v3_norm(v3_sub(vars.sample, vars.frame.origin));
			app->framebuffer[y * app->image->width + x]
				= vec3_to_rgba(trace_pixel(&app->scene,
						ray(vars.frame.origin, vars.dir), app->show_normals));
		}
	}
}
