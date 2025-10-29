#include <sys/time.h>
#include "../../include/app.h"
#include "../../include/render.h"
#include "../../include/camera.h"
#include "../../include/ui.h"

static double	stats_time_diff_ms(const struct timeval *start,
		const struct timeval *end)
{
	return ((double)(end->tv_sec - start->tv_sec)) * 1000.0
		+ ((double)(end->tv_usec - start->tv_usec)) / 1000.0;
}

void	app_render_frame(t_app *app)
{
	t_cam_frame	fr;

	if (!app || !app->image || !app->framebuffer)
		return ;
	if (app->stats.frame_count == 0)
		gettimeofday(&app->stats.app_start, NULL);
	gettimeofday(&app->stats.last_start, NULL);
	render_scene(app);
	gettimeofday(&app->stats.last_end, NULL);
	app->stats.last_ms = stats_time_diff_ms(&app->stats.last_start,
		&app->stats.last_end);
	app->stats.total_ms += app->stats.last_ms;
	app->stats.frame_count++;
	if (app->stats.frame_count > 0)
		app->stats.avg_ms = app->stats.total_ms
			/ (double)app->stats.frame_count;
	if (app->stats.last_ms > 0.0)
		app->stats.last_fps = 1000.0 / app->stats.last_ms;
	else
		app->stats.last_fps = 0.0;
	upload_framebuffer(app->image, app->framebuffer);
	if (app->overlay.visible)
	{
		camera_build_frame(&app->scene.camera, app->image->width,
			app->image->height, &fr);
		ti_show_axes(&app->overlay, &fr);
		ti_update_info(app);
	}
}
