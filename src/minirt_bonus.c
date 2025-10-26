#include "../include/minirt.h"
#include "../include/scene_bonus.h"
#include "../include/parser_bonus.h"
#include "../include/render.h"
#include "../include/hit_bonus.h"
#include "../include/shading_bonus.h"
#include "../include/app.h"

static void render_and_present(t_app *app)
{
	render_scene(app);
	upload_framebuffer(app->image, app->framebuffer);
}

static int init_window(t_app *app)
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

static void cleanup(t_app *app)
{
	if (app->overlay.overlay && app->mlx)
		mlx_delete_image(app->mlx, app->overlay.overlay);
	if (app->overlay.buffer)
	{
		free(app->overlay.buffer);
		app->overlay.buffer = NULL;
	}
	if (app->image && app->mlx)
		mlx_delete_image(app->mlx, app->image);
	if (app->mlx)
		mlx_terminate(app->mlx);
	scene_free(&app->scene);
	free(app->framebuffer);
}

int	main(int ac, char **av)
{
	t_app			app;
	t_parse_result	pr;

	if (ac != 2)
	{
		ft_putstr_fd((char *)"Usage: ", 2);
		if (av[0])
			ft_putstr_fd(av[0], 2);
		ft_putstr_fd((char *)" <scene.rt>\n", 2);
		return (1);
	}
	ft_bzero(&app, sizeof(app));
	scene_init(&app.scene);
	pr = parse_scene(av[1], &app.scene);
	if (!pr.ok)
	{
		ft_putstr_fd((char *)"Error\n", 2);
		if (pr.message)
		{
			ft_putstr_fd(pr.message, 2);
			ft_putstr_fd((char *)"\n", 2);
		}
		else
			ft_putstr_fd((char *)"parse failed\n", 2);
		parse_result_free(&pr);
		scene_free(&app.scene);
		return (EXIT_FAILURE);
	}
	parse_result_free(&pr);
	app.framebuffer = (uint32_t *)malloc(sizeof(uint32_t) * (size_t)WIN_W * (size_t)WIN_H);
	if (!app.framebuffer)
	{
		ft_putstr_fd((char *)"Error\nfailed to allocate framebuffer\n", 2);
		scene_free(&app.scene);
		return (1);
	}
	if (init_window(&app) < 0)
	{
		cleanup(&app);
		return (1);
	}
	ti_init(&app.overlay, app.mlx, app.image);
	app.show_normals = 0;
	render_and_present(&app);
	mlx_key_hook(app.mlx, &app_on_key, &app);
	mlx_loop(app.mlx);
	cleanup(&app);
	return (0);
}
