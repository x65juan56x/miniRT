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
/*
* Purpose: Trace a ray through the scene and return the pixel color.
* Logic: Find the nearest hit; if show_normals is active, visualize the normal.
*        Otherwise, compute shading using Lambert (diffuse) model.
* Notes: Normals are mapped from [-1,1] to [0,1] for RGB visualization.
* Use: Called per pixel; hit contains intersection data or miss (hit.ok = 0).
*/

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
/*
* Purpose: Render the entire scene by tracing a ray for each pixel.
* Logic: Build camera frame; iterate over all pixels (x, y) in raster space.
* -Compute normalized viewport coordinates (u, v) with 0.5 offset for
	pixel center.
* -Calculate ray direction from camera origin through the viewport sample point.
* -Trace the ray and store the resulting color in the framebuffer.
* Notes: v is flipped (1 - v) to match image space (top-left origin)
		with viewport (bottom-left).
*    Framebuffer is a 1D array indexed by (y * width + x).
* Use: Called when rendering a frame; app->show_normals toggles normal
	visualization mode.
*/
