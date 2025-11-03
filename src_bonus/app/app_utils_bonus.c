#include "../../include_bonus/minirt_bonus.h"
#include "../../include_bonus/app_bonus.h"
#include "../../include_bonus/render_bonus.h"

int	init_window(t_app *app)
{
	app->mlx = mlx_init(WIN_W, WIN_H, "miniRT", false);
	if (!app->mlx)
	{
		ft_putstr_fd((char *)"Error\nmlx_init failed\n", 2);
		return (-1);
	}
	app->image = mlx_new_image(app->mlx, WIN_W, WIN_H);
	if (!app->image)
	{
		ft_putstr_fd((char *)"Error\nmlx_new_image failed\n", 2);
		return (-1);
	}
	if (mlx_image_to_window(app->mlx, app->image, 0, 0) < 0)
	{
		ft_putstr_fd((char *)"Error\nmlx_image_to_window failed\n", 2);
		return (-1);
	}
	return (0);
}

void	cleanup(t_app *app)
{
	if (app->image && app->mlx)
		mlx_delete_image(app->mlx, app->image);
	if (app->mlx)
		mlx_terminate(app->mlx);
	scene_free(&app->scene);
	free(app->framebuffer);
}
