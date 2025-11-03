#ifndef INTERSECT_INTERNAL_BONUS_H
# define INTERSECT_INTERNAL_BONUS_H

# include "minirt_bonus.h"

/* intersect_common_bonus */
void	set_common_hit(t_hit *dst, t_common_hit *c_hit);
void	orient_normal(t_hit *hit, t_ray r);
void	apply_specular(t_hit *out, const t_material *material);

/*intersect_sphere_bonus.c */
int		record_sphere(t_sphere *sp, t_ray r, float t, t_hit *out);

/* intersect_plane_bonus.c */
int		record_plane(t_plane *pl, t_ray r, float t, t_hit *out);

/* intersect_triangle_bonus.c */
int		record_triangle(t_triangle *tr, t_ray r, float t, t_hit *out);

/* intersect_hparaboloid_bonus.c */
int		record_hparaboloid(const t_hparab *hp, t_ray r, float t, t_hit *out);

/* src_bonus/core/intersect_cylinder_bonus.c */
int		record_cylinder(t_cyl *cy, t_ray r, float t, t_hit *out);
void	cy_hit_wall(t_cyl *cy, t_common_hit *c_hit, t_hit *out);
void	cy_hit_top(t_cyl *cy, t_common_hit *c_hit, t_hit *out);
void	cy_hit_bottom(t_cyl *cy, t_common_hit *c_hit, t_hit *out);

#endif
