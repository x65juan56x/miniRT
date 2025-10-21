#include <stdlib.h>
#include <math.h>
#include "../../libraries/MLX42/include/MLX42/MLX42.h"
#include "../../include/bump_bonus.h"
#include "../../include/vec3.h"

static float luminance_u8(uint8_t r, uint8_t g, uint8_t b)
{
    // Simple average to keep it cheap
    return (float)(r + g + b) / (255.0f * 3.0f);
}

t_bumpmap *bump_load_png(const char *path)
{
    mlx_texture_t   *tex;
    t_bumpmap       *bm;
    size_t          i;
    const uint8_t   *px;

    tex = mlx_load_png(path);
    if (!tex)
        return (NULL);
    bm = (t_bumpmap *)malloc(sizeof(t_bumpmap));
    if (!bm)
    {
        mlx_delete_texture(tex);
        return (NULL);
    }
    bm->w = (int)tex->width;
    bm->h = (int)tex->height;
    bm->hmap = (float *)malloc(sizeof(float) * (size_t)bm->w * (size_t)bm->h);
    if (!bm->hmap)
    {
        mlx_delete_texture(tex);
        free(bm);
        return (NULL);
    }
    px = tex->pixels;
    i = 0;
    while (i < (size_t)bm->w * (size_t)bm->h)
    {
        uint8_t r = px[i * 4 + 0];
        uint8_t g = px[i * 4 + 1];
        uint8_t b = px[i * 4 + 2];
        bm->hmap[i] = luminance_u8(r, g, b);
        i++;
    }
    mlx_delete_texture(tex);
    return (bm);
}

void    bump_free(t_bumpmap *bm)
{
    if (!bm)
        return ;
    if (bm->hmap)
        free(bm->hmap);
    free(bm);
}

static float fractf(float x)
{
    return x - floorf(x);
}

float   bump_sample(const t_bumpmap *bm, float u, float v)
{
    int iu;
    int iv;
    if (!bm || !bm->hmap || bm->w <= 0 || bm->h <= 0)
        return 0.5f;
    // Wrap repeat
    u = fractf(u);
    v = fractf(v);
    if (u < 0.0f) u += 1.0f;
    if (v < 0.0f) v += 1.0f;
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
    return bm->hmap[(size_t)iv * (size_t)bm->w + (size_t)iu];
}

void    bump_perturb(const t_bumpmap *bm, float u, float v,
                    t_vec3 tangent, t_vec3 bitangent, float strength,
                    t_vec3 *n)
{
    float du;
    float dv;
    float h_c;
    float h_u1;
    float h_v1;
    t_vec3 dn;

    if (!bm || !n)
        return ;
    du = 1.0f / (float)bm->w;
    dv = 1.0f / (float)bm->h;
    h_c = bump_sample(bm, u, v);
    h_u1 = bump_sample(bm, u + du, v);
    h_v1 = bump_sample(bm, u, v + dv);
    // Gradient approximately (h_u - h_c)/du , (h_v - h_c)/dv
    // Use negative to emulate height-to-normal convention
    dn = v3_add(v3_mul(tangent, -(h_u1 - h_c) / (du + 1e-6f)),
               v3_mul(bitangent, -(h_v1 - h_c) / (dv + 1e-6f)));
    dn = v3_mul(dn, strength);
    *n = v3_norm(v3_add(*n, dn));
}
