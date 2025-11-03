#ifndef SHADING_BONUS_H
# define SHADING_BONUS_H

# include "hit_bonus.h"

struct					s_scene;
struct					s_light;
typedef struct s_scene	t_scene;
typedef struct s_light	t_light;

typedef struct s_light_sample
{
	t_vec3	dir;
	float	ndotl;
}	t_light_sample;

typedef struct s_spec_vars
{
	t_vec3	view_dir;
	float	spec_angle;
	float	spec_intensity;
	t_vec3	light_intensity;
}	t_spec_vars;

t_vec3	shade_lambert(const t_scene *scene, const t_hit *hit);
int		in_shadow(const t_scene *scene, const t_hit *hit, t_vec3 l_pos);
t_vec3	specular_blinn_phong(const t_scene *scene, const t_light *light,
			const t_hit *hit, t_vec3 light_dir);
t_vec3	specular_phong(const t_scene *scene, const t_light *light,
			const t_hit *hit, t_vec3 light_dir);

#endif