#ifndef HIT_H
# define HIT_H

# include "vec3.h"
# include "ray.h"
# include "scene.h"
# include "material_bonus.h"

typedef struct s_hit
{
	int			ok;
	float		t; // Distancia al primer objeto intersectado
	t_vec3		p; // Punto de impacto
	t_vec3		n; // Normal en el punto
	t_material	material;
}	t_hit;

#endif

int  scene_hit(const struct s_scene *scene, t_ray r, float t_min, float t_max, t_hit *out);