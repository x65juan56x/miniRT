/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app_bonus.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:11:46 by anagarri          #+#    #+#             */
/*   Updated: 2025/11/04 16:11:47 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
