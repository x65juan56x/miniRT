#include "../include/minirt.h"
#include <math.h>
#include <string.h>

#define FRAMEBUFFER_SIZE ((size_t)WIN_W * (size_t)WIN_H)

typedef struct s_app
{
	mlx_t		*mlx;
	mlx_image_t	*image;
	uint32_t	*framebuffer;
}t_app;

static float	clampf(float value, float min, float max)
{
	if (value < min)
		return (min);
	if (value > max)
		return (max);
	return (value);
}

static uint32_t	vec3_to_rgba(t_vec3 rgb)
{
	int	r;
	int	g;
	int	b;

	r = (int)(clampf(rgb.x, 0.0f, 1.0f) * 255.0f + 0.5f);
	g = (int)(clampf(rgb.y, 0.0f, 1.0f) * 255.0f + 0.5f);
	b = (int)(clampf(rgb.z, 0.0f, 1.0f) * 255.0f + 0.5f);
	return (rgba_u32(clamp_u8i(r), clamp_u8i(g), clamp_u8i(b), 255));
}
/*
El +0.5f en la fórmula:
Se usa para redondear correctamente el valor flotante al entero más cercano.
Sin el +0.5f, el cast a (int) simplemente trunca (corta decimales hacia abajo), lo que puede sesgar los colores hacia valores menores.
Por ejemplo:
Si el resultado es 127.8, con +0.5f → 128.3 → (int) → 128 (correcto).
Si el resultado es 127.2, con +0.5f → 127.7 → (int) → 127.
Es una técnica estándar para convertir de float a int con redondeo en vez de truncamiento.
*/
static void	generate_background(uint32_t *fb, int width, int height)
{
	const t_vec3	origin = v3(0.0f, 0.0f, 0.0f);
	const t_vec3	lower_left = v3(-2.0f, -1.0f, -1.0f);
	const t_vec3	horizontal = v3(4.0f, 0.0f, 0.0f);
	const t_vec3	vertical = v3(0.0f, 2.0f, 0.0f);
	const t_vec3	white = v3(1.0f, 1.0f, 1.0f);
	const t_vec3	blue = v3(0.5f, 0.7f, 1.0f);
	int			x;
	int			y;

	y = 0;
	while (y < height)
	{
		x = 0;
		while (x < width)
		{
			float	u = (float)x / (float)(width - 1);
			float	v = (float)(height - 1 - y) / (float)(height - 1);
			t_vec3	dir = v3_add(lower_left, v3_add(v3_mul(horizontal, u), v3_mul(vertical, v)));
			t_ray	ray_sample = ray(origin, dir);
			t_vec3	unit_dir = v3_norm(ray_sample.dir);
			float	t = 0.5f * (unit_dir.y + 1.0f);
			t_vec3	color = v3_add(v3_mul(white, 1.0f - t), v3_mul(blue, t));

			fb[y * width + x] = vec3_to_rgba(color);
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
	app->mlx = mlx_init(WIN_W, WIN_H, "miniRT playground", false);
	if (!app->mlx)
		return (fprintf(stderr, "mlx_init failed\n"), -1);
	app->image = mlx_new_image(app->mlx, WIN_W, WIN_H);
	if (!app->image)
	{
		fprintf(stderr, "mlx_new_image failed\n");
		mlx_terminate(app->mlx);
		app->mlx = NULL;
		return (-1);
	}
	if (mlx_image_to_window(app->mlx, app->image, 0, 0) < 0)
	{
		fprintf(stderr, "mlx_image_to_window failed\n");
		mlx_delete_image(app->mlx, app->image);
		mlx_terminate(app->mlx);
		app->image = NULL;
		app->mlx = NULL;
		return (-1);
	}
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

int	main(void)
{
	t_app	app;

	memset(&app, 0, sizeof(app));
	app.framebuffer = malloc(sizeof(uint32_t) * FRAMEBUFFER_SIZE);
	if (!app.framebuffer)
		return (fprintf(stderr, "Failed to allocate framebuffer\n"), EXIT_FAILURE);
	generate_background(app.framebuffer, WIN_W, WIN_H);
	if (init_window(&app) < 0)
		return (free(app.framebuffer), EXIT_FAILURE);
	upload_framebuffer(app.image, app.framebuffer);
	mlx_key_hook(app.mlx, &on_key, &app);
	mlx_loop(app.mlx);
	cleanup_app(&app);
	return (EXIT_SUCCESS);
}
