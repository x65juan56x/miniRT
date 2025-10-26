#include <math.h>
#include "../../include/finalize_bonus.h"

void    finalize_plane(t_plane *pl)
{
    t_vec3  up;

    up = v3(0.0f, 1.0f, 0.0f);
    if (fabsf(v3_dot(pl->normal, up)) > 0.999f)
        up = v3(1.0f, 0.0f, 0.0f);
    pl->u = v3_norm(v3_cross(up, pl->normal));
    pl->v = v3_cross(pl->normal, pl->u);
}

void    finalize_triangle(t_triangle *tr)
{
    t_vec3  e1;
    t_vec3  e2;

    e1 = v3_sub(tr->b, tr->a);
    e2 = v3_sub(tr->c, tr->a);
    tr->u = v3_norm(e1);
    tr->v = v3_norm(v3_sub(e2, v3_mul(tr->u, v3_dot(e2, tr->u))));
}

void    finalize_hparab(t_hparab *hp)
{
    t_vec3  up;

    up = v3(0.0f, 1.0f, 0.0f);
    if (fabsf(v3_dot(hp->axis, up)) > 0.999f)
        up = v3(1.0f, 0.0f, 0.0f);
    hp->u = v3_norm(v3_cross(up, hp->axis));
    hp->v = v3_norm(v3_cross(hp->axis, hp->u));
    hp->half_height = hp->height;
    hp->inv_rx2 = 1.0f / (hp->rx * hp->rx);
    hp->inv_ry2 = 1.0f / (hp->ry * hp->ry);
    hp->inv_height = 1.0f / hp->height;
}
