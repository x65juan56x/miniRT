#include "camera_test.h"
#include "../../include/parser.h"
#include <math.h>
#include <string.h>

#define FRAMEBUFFER_SIZE ((size_t)WIN_W * (size_t)WIN_H)

typedef struct s_app
{
	mlx_t		*mlx;
	mlx_image_t	*image;
	uint32_t	*framebuffer;
}	t_app;

static float	hit_sphere(const t_sphere *sphere, t_ray r)
{
	t_vec3	oc;
	float	a;
	float	b;
	float	c;
	float	disc;
	float	t;
	float	radius;

	radius = sphere->di * 0.5f;
	oc = v3_sub(r.orig, sphere->center);
	a = v3_dot(r.dir, r.dir);
	b = v3_dot(oc, r.dir);
	c = v3_dot(oc, oc) - radius * radius;
	disc = b * b - a * c;
	if (disc < 0.0f)
		return (-1.0f);
	t = (-b - sqrtf(disc)) / a;
	if (t > 0.0f)
		return (t);
	t = (-b + sqrtf(disc)) / a;
	if (t > 0.0f)
		return (t);
	return (-1.0f);
}
/*
* Esta nueva versión de hit_sphere considera la posibilidad de que la cámara
* esté adentro de la esfera.
*/

static float	hit_plane(const t_plane *plane, t_ray r)
{
	float	denom;
	float	t;
	t_vec3	po;

	denom = v3_dot(plane->normal, r.dir);
	if (fabsf(denom) < 1e-6f)
		return (-1.0f);
	po = v3_sub(plane->point, r.orig);
	t = v3_dot(po, plane->normal) / denom;
	if (t > 0.0f)
		return (t);
	return (-1.0f);
}

void	trace_closest(const t_scene *scene, t_ray r, t_vec3 *out_col)
{
	const t_object	*o;
	float			t;
	float			best_t;

	best_t = 1e30;
	*out_col = v3(0.0f, 0.0f, 0.0f);
	o = scene->objects;
	while (o)
	{
		t = -1.0f;
		if (o->type == OBJ_SPHERE)
			t = hit_sphere(&o->u_obj.sp, r);
		else if (o->type == OBJ_PLANE)
			t = hit_plane(&o->u_obj.pl, r);
		if (t > 0.0f && t < best_t)
		{
			best_t = t;
			if (o->type == OBJ_SPHERE)
				*out_col = o->u_obj.sp.color;
			else if (o->type == OBJ_PLANE)
				*out_col = o->u_obj.pl.color;
		}
		o = o->next;
	}
	return ;
}

static t_vec3	ray_color(t_ray r, const t_scene *scene)
{
	t_vec3			col;

	trace_closest(scene, r, &col);
	return (col);
}

static void	render_scene(uint32_t *fb, int width, int height, const t_scene *scene)
{
	t_cam_frame	frame;
	int			y;
	int			x;
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
					v3_add(v3_mul(frame.horizontal, u),
					v3_mul(frame.vertical, v)));
			dir = v3_sub(sample, frame.origin);
			rayp = ray(frame.origin, v3_norm(dir));
			fb[y * width + x] = vec3_to_rgba(ray_color(rayp, scene));
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
	app->mlx = mlx_init(WIN_W, WIN_H, "many objects playground", false);
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

static int	load_scene(const char *path, t_scene *scene)
{
	t_parse_result	result;

	result = parse_scene(path, scene);
	if (!result.ok)
	{
		if (result.message)
			ft_putendl_fd(result.message, 2);
		parse_result_free(&result);
		return (0);
	}
	parse_result_free(&result);
	return (1);
}

int	main(int ac, char **av)
{
	const char	*scene_path;
	t_app		app;
	t_scene		scene;

	if (ac != 2)
		return (1);
	scene_path = av[1];
	ft_memset(&app, 0, sizeof(app));
	scene_init(&scene);
	if (!load_scene(scene_path, &scene))
		return (1);
	app.framebuffer = malloc(sizeof(uint32_t) * FRAMEBUFFER_SIZE);
	init_window(&app);
	render_scene(app.framebuffer, WIN_W, WIN_H, &scene);
	upload_framebuffer(app.image, app.framebuffer);
	mlx_key_hook(app.mlx, &on_key, &app);
	mlx_loop(app.mlx);
	cleanup_app(&app);
	scene_free(&scene);
	return (EXIT_SUCCESS);
}
