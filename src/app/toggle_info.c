#include <stdlib.h>
#include "../../include/ui.h"
#include "../../include/vec3.h"
#include "../../include/render.h"
#include "../../libraries/libft/libft.h"

static int	ti_abs(int value)
{
	if (value < 0)
		return (-value);
	return (value);
}

static int ti_inside(int x, int y, int w, int h)
{
	return (x >= 0 && x < w && y >= 0 && y < h);
}

static void	ti_draw_line(uint32_t *fb, int w, int h,
		int x0, int y0, int x1, int y1, uint32_t color)
{
	int	dx;
	int	dy;
	int	sx;
	int	sy;
	int	err;
	int	e2;

	dx = ti_abs(x1 - x0);
	dy = ti_abs(y1 - y0);
	sx = -1;
	if (x0 <= x1)
		sx = 1;
	sy = -1;
	if (y0 <= y1)
		sy = 1;
	err = -dy / 2;
	if (dx > dy)
		err = dx / 2;
	while (1)
	{
		if ((unsigned int)x0 < (unsigned int)w
			&& (unsigned int)y0 < (unsigned int)h)
			fb[(size_t)y0 * (size_t)w + (size_t)x0] = color;
		if (x0 == x1 && y0 == y1)
			break ;
		e2 = err;
		if (e2 > -dy)
		{
			err -= dy;
			if (x0 != x1)
				x0 += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			if (y0 != y1)
				y0 += sy;
		}
	}
}

static int	ti_project(const t_cam_frame *fr, t_vec3 point,
		int w, int h, int *ox, int *oy)
{
	t_vec3	v;
	float	z;
	float	xc;
	float	yc;
	float	half_w;
	float	half_h;
	t_vec3	center;
	float	focal;
	float	x_img;
	float	y_img;
	float	u;
	float	vndc;

	v = v3_sub(point, fr->origin);
	z = v3_dot(v, fr->forward);
	if (z <= 1e-6f)
		return (0);
	xc = v3_dot(v, fr->right);
	yc = v3_dot(v, fr->up);
	half_w = v3_len(fr->horizontal) * 0.5f;
	half_h = v3_len(fr->vertical) * 0.5f;
	center = v3_add(fr->lower_left,
			v3_add(v3_mul(fr->horizontal, 0.5f), v3_mul(fr->vertical, 0.5f)));
	focal = v3_dot(v3_sub(center, fr->origin), fr->forward);
	if (focal <= 0.0f)
		return (0);
	if (half_w <= 0.0f || half_h <= 0.0f)
		return (0);
	x_img = (xc * focal) / z;
	y_img = (yc * focal) / z;
	u = (x_img + half_w) / (2.0f * half_w);
	vndc = (y_img + half_h) / (2.0f * half_h);
	*ox = (int)(u * (float)w);
	*oy = (int)((1.0f - vndc) * (float)h);
	return (1);
}

static void	ti_draw_axis(t_toggle_info *ti, const t_cam_frame *fr,
	int origin_x, int origin_y, t_vec3 axis, uint32_t color)
{
	float	step;
	float	inside_t;
	float	outside_t;
	int	end_x;
	int	end_y;
	int	px;
	int	py;
	int	found_inside;
	int	found_outside;
	int	iter;
	float	min_step;
	t_vec3	point;

	step = AXIS_LENGTH;
	inside_t = 0.0f;
	outside_t = 0.0f;
	end_x = origin_x;
	end_y = origin_y;
	found_inside = 0;
	found_outside = 0;
	min_step = 0.01f;
	iter = 0;
	while (iter < 24)
	{
		point = v3_mul(axis, step);
		if (!ti_project(fr, point, ti->w, ti->h, &px, &py))
		{
			if (!found_inside)
			{
				step *= 0.5f;
				if (step < min_step)
					break ;
			}
			else
			{
				outside_t = step;
				found_outside = 1;
				break ;
			}
			iter++;
			continue ;
		}
		if (ti_inside(px, py, ti->w, ti->h))
		{
			inside_t = step;
			end_x = px;
			end_y = py;
			found_inside = 1;
			step *= 2.0f;
			iter++;
			continue ;
		}
		if (!found_inside)
		{
			step *= 0.5f;
			if (step < min_step)
				break ;
		}
		else
		{
			outside_t = step;
			found_outside = 1;
			break ;
		}
		iter++;
	}
	if (!found_inside)
		return ;
	if (found_outside)
	{
		float	low;
		float	high;
		int	k;

		low = inside_t;
		high = outside_t;
		for (k = 0; k < 20; ++k)
		{
			float mid = (low + high) * 0.5f;
			if (!ti_project(fr, v3_mul(axis, mid), ti->w, ti->h, &px, &py))
			{
				high = mid;
				continue ;
			}
			if (ti_inside(px, py, ti->w, ti->h))
			{
				end_x = px;
				end_y = py;
				low = mid;
			}
			else
				high = mid;
		}
	}
	ti_draw_line(ti->buffer, ti->w, ti->h,
		origin_x, origin_y, end_x, end_y, color);
}

static void	ti_draw_axes(t_toggle_info *ti, const t_cam_frame *fr)
{
	int	origin_x;
	int	origin_y;

	if (!ti_project(fr, v3(0.0f, 0.0f, 0.0f), ti->w, ti->h,
			&origin_x, &origin_y))
		return ;
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(1.0f, 0.0f, 0.0f),
		AXIS_COLOR_X);
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(-1.0f, 0.0f, 0.0f),
		AXIS_COLOR_NEG_X);
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(0.0f, 1.0f, 0.0f),
		AXIS_COLOR_Y);
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(0.0f, -1.0f, 0.0f),
		AXIS_COLOR_NEG_Y);
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(0.0f, 0.0f, 1.0f),
		AXIS_COLOR_Z);
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(0.0f, 0.0f, -1.0f),
		AXIS_COLOR_NEG_Z);
}

void	ti_init(t_toggle_info *ti, mlx_t *mlx, mlx_image_t *img)
{
	ti->mlx = mlx;
	ti->overlay = mlx_new_image(mlx, img->width, img->height);
	ti->w = 0;
	ti->h = 0;
	ti->buffer = NULL;
	ti->visible = 0;
	if (!ti->overlay)
		return ;
	ti->w = (int)ti->overlay->width;
	ti->h = (int)ti->overlay->height;
	ti->buffer = (uint32_t *)malloc(sizeof(uint32_t)
			* (size_t)ti->w * (size_t)ti->h);
	if (!ti->buffer)
	{
		mlx_delete_image(mlx, ti->overlay);
		ti->overlay = NULL;
		return ;
	}
	ft_memset(ti->buffer, 0,
		(size_t)ti->w * (size_t)ti->h * sizeof(uint32_t));
	upload_framebuffer(ti->overlay, ti->buffer);
	int32_t instance_id = mlx_image_to_window(mlx, ti->overlay, 0, 0);
	if (instance_id < 0)
	{
		mlx_delete_image(mlx, ti->overlay);
		ti->overlay = NULL;
		free(ti->buffer);
		ti->buffer = NULL;
		return ;
	}
	ti->overlay->instances[instance_id].z = 1;
	ti->overlay->instances[instance_id].enabled = false;
}

void	ti_hide(t_toggle_info *ti)
{
	uint32_t	index;

	if (!ti->overlay || !ti->buffer)
		return ;
	ft_memset(ti->buffer, 0,
		(size_t)ti->w * (size_t)ti->h * sizeof(uint32_t));
	upload_framebuffer(ti->overlay, ti->buffer);
	index = 0;
	while (index < ti->overlay->count)
	{
		ti->overlay->instances[index].enabled = false;
		index++;
	}
	ti->visible = 0;
}

void	ti_show_axes(t_toggle_info *ti, const t_cam_frame *fr)
{
	uint32_t	index;

	if (!ti->overlay || !ti->buffer)
		return ;
	ft_memset(ti->buffer, 0,
		(size_t)ti->w * (size_t)ti->h * sizeof(uint32_t));
	ti_draw_axes(ti, fr);
	upload_framebuffer(ti->overlay, ti->buffer);
	index = 0;
	while (index < ti->overlay->count)
	{
		ti->overlay->instances[index].enabled = true;
		index++;
	}
	ti->visible = 1;
}
