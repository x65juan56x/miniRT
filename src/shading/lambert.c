#include "../../include/minirt.h"
#include "../../include/shading.h"
#include "../../include/scene.h"

int v3_is_zero(t_vec3 a)
{
	float eps = 1e-6;
	if(fabs(a.x) < eps && fabs(a.y) < eps && fabs(a.z) < eps)
		return 1;
	return 0;
}

t_vec3	shade_lambert(const t_scene *scene, const t_hit *hit)
{
	int i = 0;
	t_vec3	ambient;
	t_vec3	l_dir;
	float	ndotl;
	t_vec3	diff;
	t_vec3	c;
	t_light	*current_l = scene->light;
	t_vec3 color_total = v3(0,0,0);
	if (!hit->ok)
		return v3(0,0,0);
	ambient = v3_mul(scene->ambient.color, scene->ambient.ratio);

	//bucle para checkear cada valor de cada luz
	//bucle para checkear si hay sombra
	while (current_l != NULL)
	{
		l_dir = v3_norm(v3_sub(current_l->pos, hit->p));
		ndotl = v3_dot(hit->n, l_dir);
		if (ndotl < 0.0f)
			ndotl = 0.0f;
		if (in_shadow(scene, hit, current_l->pos))
		{
			current_l= current_l->next;
			continue;
		}
		//return (v3_ctoc(hit->albedo, ambient));
		diff = v3_mul(v3_mul(current_l->color, current_l->bright), ndotl);
		color_total = v3_add(color_total, diff);
		printf("procesando luz nº %i\n", i);
		i++;
		current_l= current_l->next;
	}
	if (v3_is_zero(color_total)) // función que verifica si el vector es (0,0,0)
    	return v3_ctoc(hit->albedo, ambient);
/* 	ndotl = v3_dot(hit->n, l_dir);
	if (ndotl < 0.0f)
		ndotl = 0.0f;
	diff = v3_mul(v3_mul(scene->light->color, scene->light->bright), ndotl); */
	c = v3_add(ambient, v3_ctoc(hit->albedo, color_total));
	return (c);
}
