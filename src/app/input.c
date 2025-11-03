#include "../../include/minirt.h"
#include "../../include/app.h"
#include "../../include/camera.h"
#include "../../include/render.h"
#include "../../include/ui.h"

void	app_on_key(mlx_key_data_t keydata, void *param)
{
	t_app		*app;

	app = (t_app *)param;
	if (keydata.key == MLX_KEY_ESCAPE && keydata.action == MLX_PRESS)
		mlx_close_window(app->mlx);
	if (keydata.key == MLX_KEY_N && keydata.action == MLX_PRESS)
	{
		app->show_normals = !app->show_normals;
		render_scene(app);
		upload_framebuffer(app->image, app->framebuffer);
	}
}
