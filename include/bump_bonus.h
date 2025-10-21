#ifndef BUMP_BONUS_H
# define BUMP_BONUS_H

# include <stdint.h>
# include "vec3.h"

typedef struct s_bumpmap
{
    int     w;
    int     h;
    float   *hmap; // grayscale heights in [0,1], row-major size w*h
}   t_bumpmap;

// Load a PNG bump map (grayscale derived from RGBA) into a float height map
t_bumpmap   *bump_load_png(const char *path);
void        bump_free(t_bumpmap *bm);

// Sample height with wrap repeat at normalized UV
float       bump_sample(const t_bumpmap *bm, float u, float v);

// Perturb normal by bump gradient along tangent/bitangent with given strength
// n will be normalized on output
void        bump_perturb(const t_bumpmap *bm, float u, float v,
                        t_vec3 tangent, t_vec3 bitangent, float strength,
                        t_vec3 *n);

#endif
