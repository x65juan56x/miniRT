#include "../include/minirt.h"
#include "../include/scene.h"
#include "../include/parser.h"
#include "../include/render.h"
#include "../include/hit.h"
#include "../include/shading.h"
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
		ft_putstr_fd((char *)"Error\nmlx_init failed\n", STDERR_FILENO);
		return (-1);
	}
	app->image = mlx_new_image(app->mlx, WIN_W, WIN_H);
	if (!app->image)
	{
		ft_putstr_fd((char *)"Error\nmlx_new_image failed\n", STDERR_FILENO);
		return (-1);
	}
	if (mlx_image_to_window(app->mlx, app->image, 0, 0) < 0)
	{
		ft_putstr_fd((char *)"Error\nmlx_image_to_window failed\n", STDERR_FILENO);
		return (-1);
	}
	return (0);
}

static void cleanup(t_app *app)
{
	if (app->overlay.overlay && app->mlx)
		mlx_delete_image(app->mlx, app->overlay.overlay);
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
		ft_putstr_fd((char *)"Usage: ", STDERR_FILENO);
		if (av[0])
			ft_putstr_fd(av[0], STDERR_FILENO);
		ft_putstr_fd((char *)" <scene.rt>\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	ft_bzero(&app, sizeof(app));
	scene_init(&app.scene);
	pr = parse_scene(av[1], &app.scene);
	if (!pr.ok)
	{
		ft_putstr_fd((char *)"Error\n", STDERR_FILENO);
		if (pr.message)
		{
			ft_putstr_fd(pr.message, STDERR_FILENO);
			ft_putstr_fd((char *)"\n", STDERR_FILENO);
		}
		else
			ft_putstr_fd((char *)"parse failed\n", STDERR_FILENO);
		parse_result_free(&pr);
		scene_free(&app.scene);
		return (EXIT_FAILURE);
	}
	parse_result_free(&pr);
	app.framebuffer = (uint32_t *)malloc(sizeof(uint32_t) * (size_t)WIN_W * (size_t)WIN_H);
	if (!app.framebuffer)
	{
		ft_putstr_fd((char *)"Error\nfailed to allocate framebuffer\n", STDERR_FILENO);
		scene_free(&app.scene);
		return (EXIT_FAILURE);
	}
	if (init_window(&app) < 0)
	{
		cleanup(&app);
		return (EXIT_FAILURE);
	}
	ti_init(&app.overlay, app.mlx, app.image);
	app.show_normals = 0;
	render_and_present(&app);
	mlx_key_hook(app.mlx, &app_on_key, &app);
	mlx_loop(app.mlx);
	cleanup(&app);
	return (EXIT_SUCCESS);
}
