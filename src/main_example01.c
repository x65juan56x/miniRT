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

static void	generate_background(uint32_t *fb, int width, int height)
{
	const t_vec3	origin = v3(0.0f, 0.0f, 0.0f);
	const t_vec3	lower_left = v3(-2.0f, -1.0f, -1.0f);
	const t_vec3	horizontal = v3(4.0f, 0.0f, 0.0f);
	const t_vec3	vertical = v3(0.0f, 2.0f, 0.0f);
	const t_vec3	white = v3(1.0f, 1.0f, 1.0f);
	const t_vec3	blue = v3(0.5f, 0.7f, 1.0f);
	float			u;
	float			v;
	t_vec3			dir;
	t_ray			ray_sample;
	t_vec3			unit_dir;
	float			t;
	t_vec3			color;
	int				x;
	int				y;

	y = 0;
	while (y < height)
	{
		x = 0;
		while (x < width)
		{
			u = (float)x / (float)(width - 1);
			v = (float)(height - 1 - y) / (float)(height - 1);
			dir = v3_add(lower_left, v3_add(v3_mul(horizontal, u), v3_mul(vertical, v)));
			ray_sample = ray(origin, dir);
			unit_dir = v3_norm(ray_sample.dir);
			t = 0.5f * (unit_dir.y + 1.0f);
			color = v3_add(v3_mul(white, 1.0f - t), v3_mul(blue, t));

			fb[y * width + x] = vec3_to_rgba(color);
			x++;
		}
		y++;
	}
}
/*
Propósito: Generar un gradiente de fondo tipo "cielo" en el framebuffer.
Lógica:
- Define la cámara en origin y el viewport con lower_left, horizontal y vertical.
- Recorre cada píxel (x, y) de la imagen.
- Calcula u y v para mapear el píxel al viewport (float para precisión).
- Calcula la dirección del rayo para ese píxel.
- Normaliza la dirección del rayo.
- Calcula t para interpolar entre blanco y azul según la altura (unit_dir.y).
- Mezcla los colores usando t (más azul arriba, más blanco abajo).
- Convierte el color a RGBA y lo guarda en el framebuffer.
Variables:
- fb: puntero al framebuffer (array de uint32_t).
- width, height: dimensiones de la imagen.
- x, y: índices de píxel.
- u, v: coordenadas normalizadas en el viewport.
- dir: dirección del rayo para el píxel.
- ray_sample: rayo desde el origen en la dirección dir.
- unit_dir: dirección normalizada del rayo.
- t: factor de interpolación vertical.
- color: color final del píxel.
*/

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
	if (!app->mlx)
		return (fprintf(stderr, "mlx_init failed\n"), -1);
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
/*
Propósito: Punto de entrada del programa, orquesta la inicialización, render y ciclo principal.
Lógica:
- Inicializa la estructura app a cero.
- Reserva memoria para el framebuffer.
- Genera el fondo en el framebuffer.
- Inicializa la ventana e imagen.
- Copia el framebuffer a la imagen.
- Registra el hook de teclado (ESC para cerrar).
- Entra en el bucle principal de MLX42.
- Al salir, libera todos los recursos.
Variables:
- app: estructura principal de la aplicación.
*/

/*
NOTAS EXTRA DE LA FUNCIÓN GENERATE_BACKGROUND:

¿Por qué las variables son const?
Las variables están declaradas como const porque:

Son constantes durante todo el render: Estos valores definen la geometría de la cámara y el viewport, que no cambian mientras se genera la imagen completa.

Optimización del compilador: Al marcarlas como const, el compilador puede:

Almacenarlas en registros en lugar de memoria
Hacer optimizaciones más agresivas
Evitar recargas innecesarias en el bucle interno
Claridad de código: Indica explícitamente que estos valores no se modifican dentro de la función.

Prevención de errores: Evita modificaciones accidentales de estos parámetros críticos.

¿Por qué esos valores específicos?
Los valores están diseñados para crear una cámara simple de prueba siguiendo el tutorial "Ray Tracing in One Weekend":

origin = v3(0.0f, 0.0f, 0.0f)
Cámara en el origen del mundo
Posición (0,0,0) es el punto más simple para empezar
Desde aquí se lanzan todos los rayos primarios
lower_left = v3(-2.0f, -1.0f, -1.0f)
Esquina inferior izquierda del viewport
Coordenadas: X=-2 (izquierda), Y=-1 (abajo), Z=-1 (frente a la cámara)
El viewport está a 1 unidad de distancia de la cámara (Z=-1)
horizontal = v3(4.0f, 0.0f, 0.0f)
Vector que define el ancho del viewport
Desde X=-2 hasta X=+2, total = 4 unidades de ancho
Solo componente X, mantiene Y y Z constantes
Cuando u=0 → borde izquierdo, u=1 → borde derecho
vertical = v3(0.0f, 2.0f, 0.0f)
Vector que define la altura del viewport
Desde Y=-1 hasta Y=+1, total = 2 unidades de altura
Solo componente Y, mantiene X y Z constantes
Cuando v=0 → borde inferior, v=1 → borde superior
Geometría del viewport resultante:
Vista mirando hacia -Z:

      Y=+1 ┌─────────────────┐ (X=+2, Y=+1, Z=-1)
           │                 │
           │    VIEWPORT     │
           │                 │
      Y=-1 └─────────────────┘ (X=+2, Y=-1, Z=-1)
          X=-2              X=+2
      
      lower_left            horizontal alcanza aquí
      (-2,-1,-1)           (-2,-1,-1) + (4,0,0) = (2,-1,-1)

Cálculo de dirección del rayo:
dir = v3_add(lower_left, v3_add(v3_mul(horizontal, u), v3_mul(vertical, v)));

Matemáticamente: dir = lower_left + u*horizontal + v*vertical

Ejemplos:

u=0, v=0: (-2,-1,-1) + 0*(4,0,0) + 0*(0,2,0) = (-2,-1,-1) → esquina inf. izq.
u=1, v=0: (-2,-1,-1) + 1*(4,0,0) + 0*(0,2,0) = (2,-1,-1) → esquina inf. der.
u=0, v=1: (-2,-1,-1) + 0*(4,0,0) + 1*(0,2,0) = (-2,1,-1) → esquina sup. izq.
u=0.5, v=0.5: (-2,-1,-1) + 0.5*(4,0,0) + 0.5*(0,2,0) = (0,0,-1) → centro

¿Por qué esta configuración específica?

1. Aspect ratio 2:1: viewport 4×2 crea una relación de aspecto específica
2. Distancia focal 1: viewport a Z=-1 da una "distancia focal" de 1 unidad
3. FOV implícito: con esta geometría, el FOV horizontal es ~107° (2*atan(2/1))
4. Simplicidad: valores enteros fáciles de entender y debuggear
5 Compatibilidad: sigue exactamente el tutorial estándar de ray tracing

Esta configuración crea una cámara de prueba básica para verificar
que el sistema de rays funciona correctamente antes de implementar cámaras más
sofisticadas con FOV configurable y aspect ratios arbitrarios.
*/
