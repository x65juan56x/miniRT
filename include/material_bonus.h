#ifndef MATERIAL_BONUS_H
# define MATERIAL_BONUS_H

# include "vec3.h"

typedef struct s_material
{
	t_vec3	albedo;
	float	ks;
	float	shininess;
}	t_material;

#endif