// ------------------------------------------------------------
// Red sphere over gradient (Ray Tracing in One Weekend – step)
// ------------------------------------------------------------
// Esfera roja centrada en (0,0,-1) con radio 0.5 sobre el mismo
// fondo de gradiente de cielo que ya teníamos. En lugar de
// pre-generar el fondo,lanzamos un rayo por píxel y
// decidimos el color según:
//  - Si el rayo intersecta la esfera -> rojo sólido.
//  - Si no -> gradiente cielo (blanco→azul) como antes.

#include "../../include/minirt.h"
#include <math.h>
#include <string.h>

#define FRAMEBUFFER_SIZE ((size_t)WIN_W * (size_t)WIN_H)

typedef struct s_app
{
	mlx_t		*mlx;
	mlx_image_t	*image;
	uint32_t	*framebuffer;
}	t_app;

// Intersección rayo-esfera simple.
// Devuelve t del impacto más cercano (>0) o -1 si no hay impacto.
static float	hit_sphere(t_ray r)
{
	t_vec3	oc;
	float	a;
	float	b;
	float	c;
	float	disc;

	oc = v3_sub(r.orig, v3(0.0f, 0.0f, -1.0f));
	a = v3_dot(r.dir, r.dir);
	b = v3_dot(oc, r.dir);
	c = v3_dot(oc, oc) - 0.5f * 0.5f;
	disc = b * b - a * c;
	if (disc < 0.0f)
		return (-1.0f);
	return ((-b - sqrtf(disc)) / a);
}

// Color del rayo: rojo atenuado por la distancia si golpea la esfera,
// fondo gradiente si no. Cuanto mayor t (más lejos el impacto),
// más oscuro el rojo.
static t_vec3	ray_color(t_ray r)
{
	const t_vec3	white = v3(1.0f, 1.0f, 1.0f);
	const t_vec3	blue = v3(0.5f, 0.7f, 1.0f);
	float			t;
	t_vec3			unit_dir;
	float			t_hit;
	float			k;
	float			attenuation;

	t_hit = hit_sphere(r);
	if (t_hit > 0.0f)
	{
		// Factor de atenuación: 1 / (1 + k * t_hit)
		// k controla cuánto se oscurece con la distancia.
		k = 1.0f;
		attenuation = 1.0f / (1.0f + k * t_hit);
		if (attenuation < 0.0f)
			attenuation = 0.0f;
		return (v3(attenuation, 0.0f, 0.0f));
	}
	unit_dir = v3_norm(r.dir);
	t = 0.5f * (unit_dir.y + 1.0f);
	return (v3_add(v3_mul(white, 1.0f - t), v3_mul(blue, t)));
}

// Render principal: genera rayos primarios y escribe el framebuffer.
static void	render_scene(uint32_t *fb, int width, int height)
{
	const t_vec3	origin = v3(0.0f, 0.0f, 0.0f);
	const t_vec3	lower_left = v3(-2.0f, -1.0f, -1.0f);
	const t_vec3	horizontal = v3(4.0f, 0.0f, 0.0f);
	const t_vec3	vertical = v3(0.0f, 2.0f, 0.0f);
	int			y;
	int			x;
	float		u;
	float		v;
	t_vec3			dir;
	t_ray			rayp;

	y = 0;
	while (y < height)
	{
		x = 0;
		while (x < width)
		{
			u = (float)x / (float)(width - 1);
			v = (float)(height - 1 - y) / (float)(height - 1);
			dir = v3_add(lower_left, v3_add(v3_mul(horizontal, u), v3_mul(vertical, v)));
			rayp = ray(origin, dir);
			fb[y * width + x] = vec3_to_rgba(ray_color(rayp));
			x++;
		}
		y++;
	}
}

// Copia framebuffer a la imagen MLX.
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
/*
Propósito: Copiar el framebuffer generado a la imagen de MLX42 para mostrarlo en pantalla.
Lógica:
- Recorre cada píxel (x, y) de la imagen.
- Usa mlx_put_pixel para escribir el color correspondiente desde el framebuffer.
Variables:
- image: imagen de MLX42 donde se dibuja.
- fb: framebuffer con los colores ya calculados.
- x, y: índices de píxel.
*/

static void	on_key(mlx_key_data_t keydata, void *param)
{
	t_app	*app;

	app = (t_app *)param;
	if (keydata.key == MLX_KEY_ESCAPE && keydata.action == MLX_PRESS)
		mlx_close_window(app->mlx);
}
/*
Propósito: Manejar eventos de teclado (cerrar ventana con ESC).
Lógica:
- Si se presiona ESC, llama a mlx_close_window para cerrar la ventana.
Variables:
- keydata: información de la tecla presionada.
- param: puntero a la app (usado para acceder a mlx).
*/

static int	init_window(t_app *app)
{
	app->mlx = mlx_init(WIN_W, WIN_H, "miniRT playground", false);
	app->image = mlx_new_image(app->mlx, WIN_W, WIN_H);
	mlx_image_to_window(app->mlx, app->image, 0, 0);
	return (0);
}
/*
Propósito: Inicializar la ventana y la imagen de MLX42.
Lógica:
- Crea la ventana con mlx_init.
- Crea la imagen con mlx_new_image.
- Muestra la imagen en la ventana con mlx_image_to_window.
Variables:
- app: estructura principal de la aplicación (contiene mlx e image).
*/

static void	cleanup_app(t_app *app)
{
	if (app->image)
		mlx_delete_image(app->mlx, app->image);
	if (app->mlx)
		mlx_terminate(app->mlx);
	free(app->framebuffer);
}
/*
Propósito: Liberar todos los recursos usados por la aplicación.
Lógica:
- Borra la imagen si existe.
- Termina MLX si la ventana existe.
- Libera el framebuffer.
Variables:
- app: estructura principal de la aplicación.
*/

int	main(void)
{
	t_app	app;

	ft_memset(&app, 0, sizeof(app));
	app.framebuffer = malloc(sizeof(uint32_t) * FRAMEBUFFER_SIZE);
	init_window(&app);
	render_scene(app.framebuffer, WIN_W, WIN_H);
	upload_framebuffer(app.image, app.framebuffer);
	mlx_key_hook(app.mlx, &on_key, &app);
	mlx_loop(app.mlx);
	cleanup_app(&app);
	return (EXIT_SUCCESS);
}
