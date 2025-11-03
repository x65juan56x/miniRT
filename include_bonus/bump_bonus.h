#ifndef BUMP_BONUS_H
# define BUMP_BONUS_H

# include <stdint.h>
# include "../include/vec3.h"

typedef struct s_bumpmap
{
	int		w;
	int		h;
	float	*hmap; // grayscale heights in [0,1], row-major size w*h
	float	du;
	float	dv;
}	t_bumpmap;

typedef struct s_bump_aux
{
	float		u;
	float		v;
	t_vec3		tangent;
	t_vec3		bitangent;
	float		strength;
}	t_bump_aux;

typedef struct s_bump_target
{
	int			*has_bump;
	float		*strength;
	t_bumpmap	**map;
}	t_bump_target;

typedef struct s_tr_bump_aux
{
	t_vec3	pa;
	float	d00;
	float	d01;
	float	d11;
	float	d20;
	float	d21;
	float	denom;
	float	vb;
	float	wb;
}	t_tr_bump_aux;

// Load a PNG bump map (grayscale derived from RGBA) into a float height map
t_bumpmap	*bump_load_png(const char *path);
void		bump_free(t_bumpmap *bm);

// Sample height with wrap repeat at normalized UV
float		bump_sample(const t_bumpmap *bm, float u, float v);

// Perturb normal by bump gradient along tangent/bitangent with given strength
// n will be normalized on output
void		bump_perturb(t_bumpmap *bm, t_bump_aux *bm_aux, t_vec3 *n);

#endif
