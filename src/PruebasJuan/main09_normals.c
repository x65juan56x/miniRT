#include "camera_test.h"
#include "../../include/parser.h"
#include "../../include/color.h"
#include <math.h>
#include <string.h>

#define FRAMEBUFFER_SIZE ((size_t)WIN_W * (size_t)WIN_H)

typedef struct s_app
{
	mlx_t			*mlx;
	mlx_image_t		*image;
	uint32_t		*framebuffer;
	int				show_normals;
	const t_scene	*scene;
}	t_app;

static float	hit_sphere(const t_sphere *sp, t_ray r)
{
	float			radius;
	t_vec3			oc;
	float			a;
	float			b;
	float			c;
	float			disc;
	float			t;

	radius = sp->di * 0.5f;
	oc = v3_sub(r.orig, sp->center);
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

static float hit_plane(const t_plane *pl, t_ray r)
{
	float	den;
	float	t;

	den = v3_dot(pl->normal, r.dir);
	if (fabsf(den) < 1e-6f)
		return (-1.0f);
	t = v3_dot(v3_sub(pl->point, r.orig), pl->normal) / den;
	if (t > 0.0f)
		return (t);
	return (-1.0f);
}

typedef struct s_hit
{
	int		ok;
	float	t; // Distancia al primer objeto intersectado
	t_vec3	p; // Punto de impacto
	t_vec3	n; // Normal en el punto
	t_vec3	albedo; // Color del objeto intersectado
}	t_hit;

static t_hit	intersect_scene(const t_scene *scene, t_ray r)
{
	const t_object	*o;
	float			best;
	t_hit			hit = {0};
	float			t;

	o = scene->objects;
	best = 1e30f;
	while (o)
	{
		t = -1.0f;
		if (o->type == OBJ_SPHERE)
			t = hit_sphere(&o->u_obj.sp, r);
		else if (o->type == OBJ_PLANE)
			t = hit_plane(&o->u_obj.pl, r);
		if (t > 0.0f && t < best)
		{
			best = t;
			hit.ok = 1;
			hit.t = t;
			hit.p = ray_at(r, t);
			if (o->type == OBJ_SPHERE)
			{
				hit.n = v3_norm(v3_sub(hit.p, o->u_obj.sp.center));
				hit.albedo = o->u_obj.sp.color;
			}
			else if (o->type == OBJ_PLANE)
			{
				hit.n = o->u_obj.pl.normal;
				hit.albedo = o->u_obj.pl.color;
			}
			if (v3_dot(hit.n, r.dir) > 0.0f)
				hit.n = v3_mul(hit.n, -1.0f);
		}
		o = o->next;
	}
	return (hit);
}

static t_vec3	shade_lambert(const t_scene *scene, const t_hit *hit)
{
	t_vec3	ambient;
	t_vec3	l_dir;
	float	ndotl;
	t_vec3	diff;
	t_vec3	c;

	if (!hit->ok)
		return (v3(0,0,0));
	// Ambiente
	ambient = v3_mul(scene->ambient.color, scene->ambient.ratio);
	// Difusa (Lambert, puntual)
	l_dir = v3_norm(v3_sub(scene->light.pos, hit->p));
	ndotl = v3_dot(hit->n, l_dir);
	if (ndotl < 0.0f)
		ndotl = 0.0f;
	diff = v3_mul(v3_mul(scene->light.color, scene->light.bright), ndotl);
	// Combinar con albedo
	c = v3_add(ambient, v3_ctoc(hit->albedo, diff));
	return (c);
}

static void render_scene(uint32_t *fb, int width, int height, const t_scene *scene, int show_normals)
{
	t_cam_frame	frame;
	int			x;
	int			y;
	float		u;
	float		v;
	t_vec3		c;
	t_vec3		sample;
	t_vec3		dir;
	t_hit		hit;

	camera_build_frame(&scene->camera, width, height, &frame);
	y = 0;
	while (y < height)
	{
		x = 0;
		while (x < width)
		{
			u = ((float)x + 0.5f) / (float)width;
			v = 1.0f - (((float)y + 0.5f) / (float)height);
			sample = v3_add(frame.lower_left, v3_add(v3_mul(frame.horizontal, u), v3_mul(frame.vertical, v)));
			dir = v3_norm(v3_sub(sample, frame.origin));
			hit = intersect_scene(scene, ray(frame.origin, dir));
			if (show_normals && hit.ok)
				c = v3_mul(v3_add(hit.n, v3(1,1,1)), 0.5f);
			else
				c = shade_lambert(scene, &hit);
			fb[y * width + x] = vec3_to_rgba(c);
			x++;
		}
		y++;
	}
}

static void	upload_framebuffer(mlx_image_t *image, const uint32_t *fb)
{
	uint32_t x;
	uint32_t y;

	y = 0;
	while (y < image->height)
	{
		x = 0;
		while (x < image->width)
		{
			mlx_put_pixel(image, x, y, fb[y * image->width + x]);
			x++;
		}
		y++;
	}
}

static void on_key(mlx_key_data_t keydata, void *param)
{
	t_app *app;

	app = (t_app *)param;
	if (keydata.key == MLX_KEY_ESCAPE && keydata.action == MLX_PRESS)
		mlx_close_window(app->mlx);
	if (keydata.key == MLX_KEY_N && keydata.action == MLX_PRESS)
	{
		app->show_normals = !app->show_normals;
		render_scene(app->framebuffer, WIN_W, WIN_H, app->scene, app->show_normals);
		upload_framebuffer(app->image, app->framebuffer);
	}
}

static int init_window(t_app *app)
{
	app->mlx = mlx_init(WIN_W, WIN_H, "main09 toggle normals (N)", false);
	app->image = mlx_new_image(app->mlx, WIN_W, WIN_H);
	mlx_image_to_window(app->mlx, app->image, 0, 0);
	return (0);
}

static void cleanup_app(t_app *app)
{
	if (app->image)
		mlx_delete_image(app->mlx, app->image);
	if (app->mlx)
		mlx_terminate(app->mlx);
	free(app->framebuffer);
}

static int load_scene_file(const char *path, t_scene *scene)
{
	t_parse_result r;

	r = parse_scene(path, scene);
	parse_result_free(&r);
	return (0);
}

int main(int ac, char **av)
{
	const char	*scene_path;
	t_app		app;
	t_scene		scene;

	if (ac != 2)
		return 1;
	scene_path = av[1];
	ft_memset(&app, 0, sizeof(app));
	scene_init(&scene);
	load_scene_file(scene_path, &scene);
	app.scene = &scene;
	app.show_normals = 0;
	app.framebuffer = malloc(sizeof(uint32_t) * FRAMEBUFFER_SIZE);
	init_window(&app);
	render_scene(app.framebuffer, WIN_W, WIN_H, &scene, app.show_normals);
	upload_framebuffer(app.image, app.framebuffer);
	mlx_key_hook(app.mlx, &on_key, &app);
	mlx_loop(app.mlx);
	cleanup_app(&app);
	scene_free(&scene);
	return (EXIT_SUCCESS);
}
