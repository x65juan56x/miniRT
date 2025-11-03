#include "../../include/minirt.h"
#include "../../include/app.h"
#include "../../include/render.h"

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
/*
* Purpose: Initialize the MLX window and image for rendering.
* Logic: Create MLX instance, allocate image buffer, attach to window.
* Notes: Returns -1 on failure; prints error messages to stderr.
*/

void	cleanup(t_app *app)
{
	if (app->image && app->mlx)
		mlx_delete_image(app->mlx, app->image);
	if (app->mlx)
		mlx_terminate(app->mlx);
	scene_free(&app->scene);
	free(app->framebuffer);
}
/*
* Purpose: Free all allocated resources before exiting.
* Logic: Delete MLX images, terminate MLX, free scene and framebuffer.
*/
