#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/app_bonus.h"
#include "../../include_bonus/camera_bonus.h"
#include "../../include_bonus/render_bonus.h"
#include "../../include_bonus/ui_bonus.h"

void	app_on_key(mlx_key_data_t keydata, void *param)
{
	t_app		*app;
	t_cam_frame	fr;

	app = (t_app *)param;
	if (keydata.key == MLX_KEY_ESCAPE && keydata.action == MLX_PRESS)
		mlx_close_window(app->mlx);
	if (keydata.key == MLX_KEY_N && keydata.action == MLX_PRESS)
	{
		app->show_normals = !app->show_normals;
		render_scene(app);
		upload_framebuffer(app->image, app->framebuffer);
		if (app->overlay.visible)
		{
			camera_build_frame(&app->scene.camera, app->image->width,
				app->image->height, &fr);
			ti_show_axes(&app->overlay, &fr);
		}
	}
	if (keydata.key == MLX_KEY_I && keydata.action == MLX_PRESS)
	{
		if (app->overlay.visible)
			ti_hide(&app->overlay);
		else
		{	
			camera_build_frame(&app->scene.camera, app->image->width,
				app->image->height, &fr);
			ti_show_axes(&app->overlay, &fr);
		}
	}
}
