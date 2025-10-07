#ifndef RAY_H
# define RAY_H

# include "vec3.h"

typedef struct s_ray
{
	t_vec3	orig;
	t_vec3	dir;
}	t_ray;

t_ray	ray(t_vec3 o, t_vec3 d);
t_vec3	ray_at(t_ray r, float t);

#endif
