#include "camera_test.h"
#include "../../include/parser.h"
#include "../../include/aux_math.h"
#include "../../include/color.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../libraries/libft/libft.h"

#define FRAMEBUFFER_SIZE ((size_t)WIN_W * (size_t)WIN_H)

typedef struct s_app
{
	mlx_t		*mlx;
	mlx_image_t	*image;
	uint32_t	*framebuffer;
}	t_app;

static float	hit_sphere(t_vec3 center, float radius, t_ray r)
{
	t_vec3	oc;
	float	a;
	float	b;
	float	c;
	float	disc;

	oc = v3_sub(r.orig, center);
	a = v3_dot(r.dir, r.dir);
	b = v3_dot(oc, r.dir);
	c = v3_dot(oc, oc) - radius * radius;
	disc = b * b - a * c;
	if (disc < 0.0f)
		return (-1.0f);
	return ((-b - sqrtf(disc)) / a);
}

static t_vec3	ray_color(t_ray r, const t_sphere *sphere)
{
	const t_vec3	white = v3(1.0f, 1.0f, 1.0f);
	const t_vec3	blue = v3(0.7f, 0.8f, 1.0f);
	float	    	hit;
	float	    	t;
	float	    	radius;
	t_vec3	    	unit_dir;

	radius = sphere->di * 0.5f;
	hit = hit_sphere(sphere->center, radius, r);
	if (hit > 0.0f)
		return (sphere->color);
	unit_dir = v3_norm(r.dir);
	t = 0.5f * (unit_dir.y + 1.0f);
	return (v3_add(v3_mul(white, 1.0f - t), v3_mul(blue, t)));
}

static void	render_scene(uint32_t *fb, int width, int height, const t_scene *scene,
	const t_sphere *sphere)
{
	t_cam_frame	frame;
	int		y;
	int		x;
	t_vec3		sample;
	t_vec3		dir;
	t_ray		rayp;
	float		u;
	float		v;

	camera_build_frame(&scene->camera, width, height, &frame);
	y = 0;
	while (y < height)
	{
		x = 0;
		while (x < width)
		{
			u = ((float)x + 0.5f) / (float)width;
			v = 1.0f - (((float)y + 0.5f) / (float)height);
			sample = v3_add(frame.lower_left,
				v3_add(v3_mul(frame.horizontal, u), v3_mul(frame.vertical, v)));
			dir = v3_sub(sample, frame.origin);
			rayp = ray(frame.origin, v3_norm(dir));
			fb[y * width + x] = vec3_to_rgba(ray_color(rayp, sphere));
			x++;
		}
		y++;
	}
}

static void	upload_framebuffer(mlx_image_t *image, const uint32_t *fb)
{
	int	y;
	int	x;

	y = 0;
	while ((uint32_t)y < image->height)
	{
		x = 0;
		while ((uint32_t)x < image->width)
		{
			mlx_put_pixel(image, x, y, fb[y * image->width + x]);
			x++;
		}
		y++;
	}
}

static void	on_key(mlx_key_data_t keydata, void *param)
{
	t_app	*app;

	app = (t_app *)param;
	if (keydata.key == MLX_KEY_ESCAPE && keydata.action == MLX_PRESS)
		mlx_close_window(app->mlx);
}

static int	init_window(t_app *app)
{
	app->mlx = mlx_init(WIN_W, WIN_H, "parser playground", false);
	app->image = mlx_new_image(app->mlx, WIN_W, WIN_H);
	mlx_image_to_window(app->mlx, app->image, 0, 0);
	return (0);
}

static void	cleanup_app(t_app *app)
{
	if (app->image)
		mlx_delete_image(app->mlx, app->image);
	if (app->mlx)
		mlx_terminate(app->mlx);
	free(app->framebuffer);
}

static int	load_scene(const char *path, t_scene *scene, const t_sphere **out_sphere)
{
	t_parse_result	result;

	result = parse_scene(path, scene);
	parse_result_free(&result);
	*out_sphere = &scene->objects->u_obj.sp;
	return (0);
}

int	main(void)
{
	const char		*scene_path;
	t_app			app;
	t_scene			scene;
	const t_sphere	*sphere;

	scene_path = "src/PruebasJuan/ejemplos/parser_playground.rt";
	ft_memset(&app, 0, sizeof(app));
	scene_init(&scene);
	sphere = NULL;
	load_scene(scene_path, &scene, &sphere);
	app.framebuffer = malloc(sizeof(uint32_t) * FRAMEBUFFER_SIZE);
	init_window(&app);
	render_scene(app.framebuffer, WIN_W, WIN_H, &scene, sphere);
	upload_framebuffer(app.image, app.framebuffer);
	mlx_key_hook(app.mlx, &on_key, &app);
	mlx_loop(app.mlx);
	cleanup_app(&app);
	scene_free(&scene);
	return (EXIT_SUCCESS);
}
