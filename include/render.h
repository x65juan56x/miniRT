#ifndef RENDER_H
# define RENDER_H

# include <stdint.h>
# include "minirt.h"
# include "camera.h"

// Forward declaration to avoid pulling app/scene into this public header
struct					s_app;
typedef struct s_app	t_app;

typedef struct s_render_aux
{
	t_cam_frame	frame;
	float		u;
	float		v;
	t_vec3		sample;
	t_vec3		dir;
}	t_render_aux;

void	render_scene(t_app *app);
void	upload_framebuffer(mlx_image_t *image, const uint32_t *fb);

#endif