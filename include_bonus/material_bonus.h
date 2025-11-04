/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_bonus.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anagarri <anagarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:12:27 by anagarri          #+#    #+#             */
/*   Updated: 2025/11/04 16:12:28 by anagarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
