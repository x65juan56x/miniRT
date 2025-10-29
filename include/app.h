#ifndef APP_H
# define APP_H

# include <sys/time.h>
# include "ui.h"

typedef struct s_render_stats
{
	struct timeval	app_start;
	struct timeval	last_start;
	struct timeval	last_end;
	double			last_ms;
	double			total_ms;
	double			avg_ms;
	double			last_fps;
	unsigned long	frame_count;
} t_render_stats;

typedef struct s_app
{
	mlx_t			*mlx;
	mlx_image_t		*image;
	uint32_t		*framebuffer;
	int				show_normals;
	t_scene			scene;
	t_toggle_info	overlay;
	t_render_stats	stats;
}	t_app;

void	app_on_key(mlx_key_data_t keydata, void *param);
void	app_render_frame(t_app *app);

#endif