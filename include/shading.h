#ifndef SHADING_H
# define SHADING_H

# include "hit.h"

t_vec3	shade_lambert(const t_scene *scene, const t_hit *hit);
int		in_shadow(const t_scene *scene, t_vec3 p, t_vec3 l_pos);

#endif