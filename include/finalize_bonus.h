#ifndef FINALIZE_BONUS_H
#define FINALIZE_BONUS_H

#include "scene_bonus.h"

/* Compute tangent bases and cached inverses for objects that need them. */
void    finalize_plane(t_plane *pl);
void    finalize_triangle(t_triangle *tr);
void    finalize_hparab(t_hparab *hp);

#endif
