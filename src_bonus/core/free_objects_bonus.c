#include "../../include_bonus/minirt_bonus.h"

void	free_sphere(t_object *it)
{
	if (it->u_obj.sp.bump)
		bump_free(it->u_obj.sp.bump);
	if (it->u_obj.sp.material)
		free(it->u_obj.sp.material);
}

void	free_plane(t_object *it)
{
	if (it->u_obj.pl.bump)
		bump_free(it->u_obj.pl.bump);
	if (it->u_obj.pl.material)
		free(it->u_obj.pl.material);
}

void	free_cylinder(t_object *it)
{
	if (it->u_obj.cy.bump)
		bump_free(it->u_obj.cy.bump);
	if (it->u_obj.cy.material)
		free(it->u_obj.cy.material);
}

void	free_triangle(t_object *it)
{
	if (it->u_obj.tr.bump)
		bump_free(it->u_obj.tr.bump);
	if (it->u_obj.tr.material)
		free(it->u_obj.tr.material);
}

void	free_hparab(t_object *it)
{
	if (it->u_obj.hp.bump)
		bump_free(it->u_obj.hp.bump);
	if (it->u_obj.hp.material)
		free(it->u_obj.hp.material);
}
