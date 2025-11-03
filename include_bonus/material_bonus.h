#ifndef MATERIAL_BONUS_H
# define MATERIAL_BONUS_H

# include "../include/vec3.h"

typedef enum e_spec_model
{
	SPEC_MODEL_BLINN,
	SPEC_MODEL_PHONG
}	t_spec_model;

typedef struct s_material
{
	t_vec3			albedo;
	float			ks;
	float			shininess;
	t_spec_model	model;
	float			reflectivity;
}	t_material;

#endif
