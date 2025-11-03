#ifndef APP_BONUS_H
# define APP_BONUS_H

# include "scene_bonus.h"

typedef struct s_app
{
	mlx_t			*mlx;
	mlx_image_t		*image;
	uint32_t		*framebuffer;
	int				show_normals;
	t_scene			scene;
}	t_app;

void	app_on_key(mlx_key_data_t keydata, void *param);

int		init_window(t_app *app);
void	cleanup(t_app *app);

#endif
