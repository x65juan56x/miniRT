#ifndef UI_H
# define UI_H

# include <stdint.h>
# include "camera.h"
# include "../libraries/MLX42/include/MLX42/MLX42.h"

# define AXIS_COLOR_X 0xFF0000FFu
# define AXIS_COLOR_Y 0x00FF00FFu
# define AXIS_COLOR_Z 0x0000FFFFu
# define AXIS_COLOR_NEG_X 0x660000FFu
# define AXIS_COLOR_NEG_Y 0x006600FFu
# define AXIS_COLOR_NEG_Z 0x000066FFu

# define AXIS_LENGTH 16.0f

typedef struct s_toggle_info
{
	mlx_t		*mlx;
	mlx_image_t	*overlay;
	uint32_t	*buffer;
	int			w;
	int			h;
	int			visible;
}	t_toggle_info;

void	ti_init(t_toggle_info *ti, mlx_t *mlx, mlx_image_t *base_img);
void	ti_hide(t_toggle_info *ti);
void	ti_show_axes(t_toggle_info *ti, const t_cam_frame *fr);

#endif
