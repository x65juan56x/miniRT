#include "toggle_info.h"
#include "../../include/vec3.h"
#include "../../libraries/libft/libft.h"

static int	ti_abs(int value)
{
	if (value < 0)
		return (-value);
	return (value);
}

static void	ti_put_pixel(uint8_t *fb, int w, int h,
		int x, int y, uint32_t color)
{
	size_t	index;

	if ((unsigned int)x < (unsigned int)w
		&& (unsigned int)y < (unsigned int)h)
	{
		index = ((size_t)y * (size_t)w + (size_t)x) * 4;
		fb[index + 0] = (uint8_t)((color >> 24) & 0xFF);
		fb[index + 1] = (uint8_t)((color >> 16) & 0xFF);
		fb[index + 2] = (uint8_t)((color >> 8) & 0xFF);
		fb[index + 3] = (uint8_t)(color & 0xFF);
	}
}

static void	ti_draw_line(uint8_t *fb, int w, int h,
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
		ti_put_pixel(fb, w, h, x0, y0, color);
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
	float	length;
	int		attempt;
	int		end_x;
	int		end_y;
	t_vec3	endpoint;

	length = AXIS_LENGTH;
	attempt = 0;
	while (attempt < 8)
	{
		endpoint = v3_mul(axis, length);
		if (ti_project(fr, endpoint, ti->w, ti->h, &end_x, &end_y))
		{
			if (ti_abs(end_x - origin_x) <= ti->w
				&& ti_abs(end_y - origin_y) <= ti->h)
			{
				ti_draw_line(ti->pixels, ti->w, ti->h,
					origin_x, origin_y, end_x, end_y, color);
				return ;
			}
		}
		length = length * 0.5f;
		if (length < 0.01f)
			break ;
		attempt++;
	}
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
	ti->pixels = NULL;
	ti->visible = 0;
	if (!ti->overlay)
		return ;
	ti->w = (int)ti->overlay->width;
	ti->h = (int)ti->overlay->height;
	ti->pixels = ti->overlay->pixels;
	ft_memset(ti->overlay->pixels, 0, (size_t)ti->w * (size_t)ti->h * 4);
	int32_t instance_id = mlx_image_to_window(mlx, ti->overlay, 0, 0);
	if (instance_id < 0)
	{
		mlx_delete_image(mlx, ti->overlay);
		ti->overlay = NULL;
		ti->pixels = NULL;
		return ;
	}
	ti->overlay->instances[instance_id].z = 1;
	ti->overlay->instances[instance_id].enabled = false;
}

void	ti_hide(t_toggle_info *ti)
{
	uint32_t	index;

	if (!ti->overlay || !ti->pixels)
		return ;
	ft_memset(ti->overlay->pixels, 0, (size_t)ti->w * (size_t)ti->h * 4);
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

	if (!ti->overlay || !ti->pixels)
		return ;
	ft_memset(ti->overlay->pixels, 0, (size_t)ti->w * (size_t)ti->h * 4);
	ti_draw_axes(ti, fr);
	index = 0;
	while (index < ti->overlay->count)
	{
		ti->overlay->instances[index].enabled = true;
		index++;
	}
	ti->visible = 1;
}
