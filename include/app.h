/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:02:32 by anagarri          #+#    #+#             */
/*   Updated: 2025/11/04 16:02:34 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APP_H
# define APP_H

# include "scene.h"

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