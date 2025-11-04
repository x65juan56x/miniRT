#include <stdlib.h>
#include <math.h>
#include "../../libraries/MLX42/include/MLX42/MLX42.h"
#include "../../include_bonus/bump_bonus.h"
#include "../../include/vec3.h"

static float	luminance_u8(uint8_t r, uint8_t g, uint8_t b)
{
	return ((float)(r + g + b) / (255.0f * 3.0f));
}
// Convert RGB color to grayscale brightness value [0,1] using simple average.

t_bumpmap	*bump_load_png(const char *path)
{
	mlx_texture_t	*tex;
	t_bumpmap		*bm;
	size_t			i;

	tex = mlx_load_png(path);
	if (!tex)
		return (NULL);
	bm = (t_bumpmap *)malloc(sizeof(t_bumpmap));
	if (!bm)
		return (mlx_delete_texture(tex), NULL);
	bm->w = (int)tex->width;
	bm->h = (int)tex->height;
	bm->hmap = (float *)malloc(sizeof(float) * (size_t)bm->w * (size_t)bm->h);
	if (!bm->hmap)
		return (mlx_delete_texture(tex), free(bm), NULL);
	i = -1;
	while (++i < (size_t)bm->w * (size_t)bm->h)
		bm->hmap[i] = luminance_u8(tex->pixels[i * 4 + 0],
				tex->pixels[i * 4 + 1], tex->pixels[i * 4 + 2]);
	return (mlx_delete_texture(tex), bm);
}
/*
* Purpose: Load a PNG image and convert it to a heightmap for bump mapping.
* Inputs: path (PNG file path).
* Algorithm:
*   - Load PNG texture using MLX library
*   - Allocate bump map structure and heightmap array
*   - Convert each pixel to grayscale (brightness represents height)
*   - Brighter pixels = higher parts, darker pixels = lower parts
*   - Clean up texture after extraction
* Returns: Bump map structure, or NULL on failure.
* Use: Called when parsing objects with bump mapping enabled.
*/

void	bump_free(t_bumpmap *bm)
{
	if (!bm)
		return ;
	if (bm->hmap)
		free(bm->hmap);
	free(bm);
}
// Free memory allocated for bump map structure and heightmap data.

float	bump_sample(const t_bumpmap *bm, float u, float v)
{
	int	iu;
	int	iv;

	if (!bm || !bm->hmap || bm->w <= 0 || bm->h <= 0)
		return (0.5f);
	u = u - floorf(u);
	v = v - floorf(v);
	if (u < 0.0f)
		u += 1.0f;
	if (v < 0.0f)
		v += 1.0f;
	iu = (int)floorf(u * (float)bm->w);
	iv = (int)floorf(v * (float)bm->h);
	if (iu < 0)
		iu = 0;
	if (iu >= bm->w)
		iu = bm->w - 1;
	if (iv < 0)
		iv = 0;
	if (iv >= bm->h)
		iv = bm->h - 1;
	return (bm->hmap[(size_t)iv * (size_t)bm->w + (size_t)iu]);
}
/*
* Purpose: Sample height value from bump map at texture coordinates (u,v).
* Inputs: bm (bump map), u and v (texture coordinates, typically [0,1]).
* Algorithm:
*   - Wrap u and v coordinates (using fractional part for tiling)
*   - Convert floating-point UV to integer pixel coordinates
*   - Clamp to valid image bounds
*   - Return height value at that pixel
* Returns: Height value [0,1], or 0.5 (flat) if map is invalid.
* Use: Called by bump_perturb to read heightmap values.
*/

void	bump_perturb(t_bumpmap *bm, t_bump_aux *bm_aux,
					t_vec3 *n)
{
	float	h_c;
	float	h_u1;
	float	h_v1;
	t_vec3	dn;

	if (!bm || !n)
		return ;
	bm->du = 1.0f / (float)bm->w;
	bm->dv = 1.0f / (float)bm->h;
	h_c = bump_sample(bm, bm_aux->u, bm_aux->v);
	h_u1 = bump_sample(bm, bm_aux->u + bm->du, bm_aux->v);
	h_v1 = bump_sample(bm, bm_aux->u, bm_aux->v + bm->dv);
	dn = v3_add(v3_mul(bm_aux->tangent, -(h_u1 - h_c) / (bm->du + 1e-6f)),
			v3_mul(bm_aux->bitangent, -(h_v1 - h_c) / (bm->dv + 1e-6f)));
	dn = v3_mul(dn, bm_aux->strength);
	*n = v3_norm(v3_add(*n, dn));
}
/*
* Purpose: Perturb surface normal using bump map to create illusion of detail.
* Inputs: bm (bump map), bm_aux (UV coords, tangent basis, strength), n (normal to modify).
* Algorithm:
*   - Sample height at current position (h_c)
*   - Sample height at nearby positions (h_u1, h_v1) using small offset (du, dv)
*   - Calculate gradient: how quickly height changes in u and v directions
*       • (h_u1 - h_c)/du: slope along tangent direction
*       • (h_v1 - h_c)/dv: slope along bitangent direction
*   - Use negative slopes to match height-to-normal convention (higher = bump out)
*   - Combine gradients using tangent basis to get normal perturbation (dn)
*   - Scale by strength and add to original normal
*   - Normalize result to get final perturbed normal
* Notes: This creates fake surface detail without changing geometry.
*        Makes flat surfaces look bumpy by changing how light reflects.
* Use: Called by surface processing functions when bump mapping is enabled.
*/
