#ifndef RENDER_H
# define  RENDER_H

# include <stdint.h>
# include "scene.h"
# include "minirt.h"

void	render_scene(uint32_t *fb, int width, int height, const t_scene *scene, int show_normals);
void	upload_framebuffer(mlx_image_t *image, const uint32_t *fb);

#endif