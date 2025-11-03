#include "../include/minirt.h"
#include "../include/scene.h"
#include "../include/parser.h"
#include "../include/render.h"
#include "../include/hit.h"
#include "../include/shading.h"
#include "../include/app.h"

static void	render_and_present(t_app *app)
{
	render_scene(app);
	upload_framebuffer(app->image, app->framebuffer);
}
/*
* Purpose: Render the scene and upload the framebuffer to the display image.
* Logic: Calls render_scene to trace rays; then uploads result to MLX image.
*/

static int	print_usage(int ac, char **av)
{
	if (ac != 2)
	{
		ft_putstr_fd((char *)"Usage: ", 2);
		if (av[0])
			ft_putstr_fd(av[0], 2);
		ft_putstr_fd((char *)" <scene.rt>\n", 2);
		return (1);
	}
	return (0);
}
/*
* Purpose: Validate argument count and print usage message if incorrect.
* Logic: Returns 1 if args invalid, 0 otherwise.
*/

static int	init_framebuffer(t_app *app)
{
	app->framebuffer = (uint32_t *)malloc(sizeof(uint32_t)
			*(size_t)WIN_W * (size_t)WIN_H);
	if (!app->framebuffer)
	{
		ft_putstr_fd((char *)"Error\nfailed to allocate framebuffer\n", 2);
		scene_free(&app->scene);
		return (0);
	}
	return (1);
}
/*
* Purpose: Allocate memory for the framebuffer (pixel color storage).
* Logic: Allocates WIN_W × WIN_H uint32_t array; returns 0 on failure.
*/

t_parse_result	init_and_parse(t_app *app, char **av)
{
	t_parse_result	pr;

	ft_bzero(app, sizeof(app));
	scene_init(&app->scene);
	pr = parse_scene(av[1], &app->scene);
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
		scene_free(&app->scene);
	}
	return (pr);
}
/*
* Purpose: Initialize app structure and parse the scene file.
* Logic: Zero app, init scene, call parser; print errors if parsing fails.
*/

int	main(int ac, char **av)
{
	t_app			app;
	t_parse_result	pr;

	if (print_usage(ac, av))
		return (1);
	pr = init_and_parse(&app, av);
	if (!pr.ok)
		return (EXIT_FAILURE);
	parse_result_free(&pr);
	if (!init_framebuffer(&app))
		return (EXIT_FAILURE);
	if (init_window(&app) < 0)
	{
		cleanup(&app);
		return (1);
	}
	app.show_normals = 0;
	render_and_present(&app);
	mlx_key_hook(app.mlx, &app_on_key, &app);
	mlx_loop(app.mlx);
	cleanup(&app);
	return (0);
}
/* int	main(int ac, char **av)
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
	app.framebuffer = (uint32_t *)malloc(sizeof(uint32_t)
			*(size_t)WIN_W * (size_t)WIN_H);
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
 */