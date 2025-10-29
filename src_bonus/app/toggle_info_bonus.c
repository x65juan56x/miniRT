#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include "../../include_bonus/ui_bonus.h"
#include "../../include_bonus/app_bonus.h"
#include "../../include/vec3.h"
#include "../../include_bonus/render_bonus.h"
#include "../../libraries/libft/libft.h"

static double	ti_time_diff_ms(const struct timeval *start,
			const struct timeval *end)
{
	return ((double)(end->tv_sec - start->tv_sec)) * 1000.0
		+ ((double)(end->tv_usec - start->tv_usec)) / 1000.0;
}

static void	ti_clear_labels(t_toggle_info *ti)
{
	size_t	index;

	if (!ti || !ti->labels)
		return ;
	index = 0;
	while (index < ti->label_count)
	{
		if (ti->labels[index] && ti->mlx)
			mlx_delete_image(ti->mlx, ti->labels[index]);
		index++;
	}
	free(ti->labels);
	ti->labels = NULL;
	ti->label_count = 0;
}

static void	ti_set_labels_visibility(t_toggle_info *ti, int visible)
{
	size_t	label;
	size_t	inst;

	if (!ti || !ti->labels)
		return ;
	label = 0;
	while (label < ti->label_count)
	{
		if (ti->labels[label])
		{
			inst = 0;
			while (inst < ti->labels[label]->count)
			{
				ti->labels[label]->instances[inst].enabled = visible;
				inst++;
			}
		}
		label++;
	}
}

static int	ti_add_label(t_toggle_info *ti, t_app *app,
		const char *text, int x, int y)
{
	mlx_image_t	*img;
	mlx_image_t	**new_array;

	img = mlx_put_string(app->mlx, text, x, y);
	if (!img)
		return (0);
	img->instances[0].z = 2;
	img->instances[0].enabled = ti->visible;
	new_array = (mlx_image_t **)malloc(sizeof(mlx_image_t *)
		* (ti->label_count + 1));
	if (!new_array)
	{
		mlx_delete_image(app->mlx, img);
		return (0);
	}
	if (ti->labels)
	{
		ft_memcpy(new_array, ti->labels,
			sizeof(mlx_image_t *) * ti->label_count);
		free(ti->labels);
	}
	new_array[ti->label_count] = img;
	ti->labels = new_array;
	ti->label_count++;
	return (1);
}

static int	ti_add_formatted_label(t_toggle_info *ti, t_app *app,
		int x, int *y, const char *fmt, ...)
{
	char		buffer[256];
	va_list	args;

	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);
	if (!ti_add_label(ti, app, buffer, x, *y))
		return (0);
	*y += 18;
	return (1);
}

static const char	*ti_object_type_name(t_objtype type)
{
	if (type == OBJ_SPHERE)
		return ("Sphere");
	if (type == OBJ_PLANE)
		return ("Plane");
	if (type == OBJ_CYLINDER)
		return ("Cylinder");
	if (type == OBJ_TRIANGLE)
		return ("Triangle");
	if (type == OBJ_HPARABOLOID)
		return ("Hyperboloid");
	return ("Unknown");
}

static int ti_abs(int value)
{
	if (value < 0)
		return (-value);
	return (value);
}

static int ti_inside(int x, int y, int w, int h)
{
	return (x >= 0 && x < w && y >= 0 && y < h);
}

static void ti_draw_line(uint32_t *fb, int w, int h,
		int x0, int y0, int x1, int y1, uint32_t color)
{
	int dx; int dy; int sx; int sy; int err; int e2;
	dx = ti_abs(x1 - x0);
	dy = ti_abs(y1 - y0);
	sx = (x0 <= x1) ? 1 : -1;
	sy = (y0 <= y1) ? 1 : -1;
	err = (dx > dy) ? dx / 2 : -dy / 2;
	while (1)
	{
		if ((unsigned int)x0 < (unsigned int)w && (unsigned int)y0 < (unsigned int)h)
			fb[(size_t)y0 * (size_t)w + (size_t)x0] = color;
		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 > -dy) { err -= dy; if (x0 != x1) x0 += sx; }
		if (e2 < dx) { err += dx; if (y0 != y1) y0 += sy; }
	}
}

static int ti_project(const t_cam_frame *fr, t_vec3 point,
		int w, int h, int *ox, int *oy)
{
	t_vec3 v; float z; float xc; float yc; float half_w; float half_h; t_vec3 center; float focal; float x_img; float y_img; float u; float vndc;
	v = v3_sub(point, fr->origin);
	z = v3_dot(v, fr->forward);
	if (z <= 1e-6f) return (0);
	xc = v3_dot(v, fr->right);
	yc = v3_dot(v, fr->up);
	half_w = v3_len(fr->horizontal) * 0.5f;
	half_h = v3_len(fr->vertical) * 0.5f;
	center = v3_add(fr->lower_left, v3_add(v3_mul(fr->horizontal, 0.5f), v3_mul(fr->vertical, 0.5f)));
	focal = v3_dot(v3_sub(center, fr->origin), fr->forward);
	if (focal <= 0.0f || half_w <= 0.0f || half_h <= 0.0f) return (0);
	x_img = (xc * focal) / z;
	y_img = (yc * focal) / z;
	u = (x_img + half_w) / (2.0f * half_w);
	vndc = (y_img + half_h) / (2.0f * half_h);
	*ox = (int)(u * (float)w);
	*oy = (int)((1.0f - vndc) * (float)h);
	return (1);
}

static void ti_draw_axis(t_toggle_info *ti, const t_cam_frame *fr,
	int origin_x, int origin_y, t_vec3 axis, uint32_t color)
{
	float step = AXIS_LENGTH; float inside_t = 0.0f; float outside_t = 0.0f; int end_x = origin_x; int end_y = origin_y; int px, py; int found_inside = 0; int found_outside = 0; int iter = 0; float min_step = 0.01f; t_vec3 point;
	while (iter < 24)
	{
		point = v3_mul(axis, step);
		if (!ti_project(fr, point, ti->w, ti->h, &px, &py))
		{
			if (!found_inside) { step *= 0.5f; if (step < min_step) break; }
			else { outside_t = step; found_outside = 1; break; }
			iter++; continue;
		}
		if (ti_inside(px, py, ti->w, ti->h))
		{
			inside_t = step; end_x = px; end_y = py; found_inside = 1; step *= 2.0f; iter++; continue;
		}
		if (!found_inside) { step *= 0.5f; if (step < min_step) break; }
		else { outside_t = step; found_outside = 1; break; }
		iter++;
	}
	if (!found_inside) return;
	if (found_outside)
	{
		float low = inside_t, high = outside_t; int k;
		for (k = 0; k < 20; ++k)
		{
			float mid = (low + high) * 0.5f;
			if (!ti_project(fr, v3_mul(axis, mid), ti->w, ti->h, &px, &py)) { high = mid; continue; }
			if (ti_inside(px, py, ti->w, ti->h)) { end_x = px; end_y = py; low = mid; }
			else high = mid;
		}
	}
	ti_draw_line(ti->buffer, ti->w, ti->h, origin_x, origin_y, end_x, end_y, color);
}

static void ti_draw_axes(t_toggle_info *ti, const t_cam_frame *fr)
{
	int origin_x, origin_y;
	if (!ti_project(fr, v3(0.0f, 0.0f, 0.0f), ti->w, ti->h, &origin_x, &origin_y)) return;
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(1.0f, 0.0f, 0.0f), AXIS_COLOR_X);
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(-1.0f, 0.0f, 0.0f), AXIS_COLOR_NEG_X);
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(0.0f, 1.0f, 0.0f), AXIS_COLOR_Y);
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(0.0f, -1.0f, 0.0f), AXIS_COLOR_NEG_Y);
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(0.0f, 0.0f, 1.0f), AXIS_COLOR_Z);
	ti_draw_axis(ti, fr, origin_x, origin_y, v3(0.0f, 0.0f, -1.0f), AXIS_COLOR_NEG_Z);
}

void ti_init(t_toggle_info *ti, mlx_t *mlx, mlx_image_t *img)
{
	ti->mlx = mlx;
	ti->overlay = mlx_new_image(mlx, img->width, img->height);
	ti->w = 0;
	ti->h = 0;
	ti->buffer = NULL;
	ti->visible = 0;
	ti->labels = NULL;
	ti->label_count = 0;
	if (!ti->overlay) return;
	ti->w = (int)ti->overlay->width; ti->h = (int)ti->overlay->height;
	ti->buffer = (uint32_t *)malloc(sizeof(uint32_t) * (size_t)ti->w * (size_t)ti->h);
	if (!ti->buffer) { mlx_delete_image(mlx, ti->overlay); ti->overlay = NULL; return; }
	ft_memset(ti->buffer, 0, (size_t)ti->w * (size_t)ti->h * sizeof(uint32_t));
	upload_framebuffer(ti->overlay, ti->buffer);
	int32_t instance_id = mlx_image_to_window(mlx, ti->overlay, 0, 0);
	if (instance_id < 0) { mlx_delete_image(mlx, ti->overlay); ti->overlay = NULL; free(ti->buffer); ti->buffer = NULL; return; }
	ti->overlay->instances[instance_id].z = 1;
	ti->overlay->instances[instance_id].enabled = false;
}

void ti_hide(t_toggle_info *ti)
{
	uint32_t index;
	if (!ti->overlay || !ti->buffer) return;
	ft_memset(ti->buffer, 0, (size_t)ti->w * (size_t)ti->h * sizeof(uint32_t));
	upload_framebuffer(ti->overlay, ti->buffer);
	index = 0;
	while (index < ti->overlay->count) { ti->overlay->instances[index].enabled = false; index++; }
	ti_set_labels_visibility(ti, 0);
	ti->visible = 0;
}

void ti_show_axes(t_toggle_info *ti, const t_cam_frame *fr)
{
	uint32_t index;
	if (!ti->overlay || !ti->buffer) return;
	ft_memset(ti->buffer, 0, (size_t)ti->w * (size_t)ti->h * sizeof(uint32_t));
	ti_draw_axes(ti, fr);
	upload_framebuffer(ti->overlay, ti->buffer);
	index = 0;
	while (index < ti->overlay->count) { ti->overlay->instances[index].enabled = true; index++; }
	ti_set_labels_visibility(ti, 1);
	ti->visible = 1;
}

static int ti_add_object_details(t_toggle_info *ti, t_app *app,
	int x, int *y, size_t index, const t_object *obj)
{
	if (!ti_add_formatted_label(ti, app, x, y, "Obj %zu %s",
		index, ti_object_type_name(obj->type)))
		return (0);
	if (obj->type == OBJ_SPHERE)
	{
		t_vec3 c = obj->u_obj.sp.center;
		t_vec3 col = obj->u_obj.sp.color;
		if (!ti_add_formatted_label(ti, app, x + 12, y,
			"center=(%.2f, %.2f, %.2f) di=%.2f color=(%.2f, %.2f, %.2f)",
			c.x, c.y, c.z, obj->u_obj.sp.di,
			col.x, col.y, col.z))
			return (0);
		if ((obj->u_obj.sp.has_checker || obj->u_obj.sp.has_bump)
			&& !ti_add_formatted_label(ti, app, x + 12, y,
				"checker=%s scale=%.2f bump=%s strength=%.2f",
				obj->u_obj.sp.has_checker ? "yes" : "no",
				obj->u_obj.sp.checker_scale,
				obj->u_obj.sp.has_bump ? "yes" : "no",
				obj->u_obj.sp.bump_strength))
			return (0);
	}
	else if (obj->type == OBJ_PLANE)
	{
		t_vec3 p = obj->u_obj.pl.point;
		t_vec3 n = obj->u_obj.pl.normal;
		t_vec3 col = obj->u_obj.pl.color;
		if (!ti_add_formatted_label(ti, app, x + 12, y,
			"point=(%.2f, %.2f, %.2f) normal=(%.2f, %.2f, %.2f) color=(%.2f, %.2f, %.2f)",
			p.x, p.y, p.z, n.x, n.y, n.z,
			col.x, col.y, col.z))
			return (0);
		if ((obj->u_obj.pl.has_checker || obj->u_obj.pl.has_bump)
			&& !ti_add_formatted_label(ti, app, x + 12, y,
				"checker=%s scale=%.2f bump=%s strength=%.2f",
				obj->u_obj.pl.has_checker ? "yes" : "no",
				obj->u_obj.pl.checker_scale,
				obj->u_obj.pl.has_bump ? "yes" : "no",
				obj->u_obj.pl.bump_strength))
			return (0);
	}
	else if (obj->type == OBJ_CYLINDER)
	{
		t_vec3 c = obj->u_obj.cy.center;
		t_vec3 a = obj->u_obj.cy.axis;
		t_vec3 col = obj->u_obj.cy.color;
		if (!ti_add_formatted_label(ti, app, x + 12, y,
			"center=(%.2f, %.2f, %.2f) axis=(%.2f, %.2f, %.2f) di=%.2f h=%.2f color=(%.2f, %.2f, %.2f)",
			c.x, c.y, c.z, a.x, a.y, a.z, obj->u_obj.cy.di,
			obj->u_obj.cy.he, col.x, col.y, col.z))
			return (0);
		if ((obj->u_obj.cy.has_checker || obj->u_obj.cy.has_bump)
			&& !ti_add_formatted_label(ti, app, x + 12, y,
				"checker=%s scale=%.2f bump=%s strength=%.2f",
				obj->u_obj.cy.has_checker ? "yes" : "no",
				obj->u_obj.cy.checker_scale,
				obj->u_obj.cy.has_bump ? "yes" : "no",
				obj->u_obj.cy.bump_strength))
			return (0);
	}
	else if (obj->type == OBJ_TRIANGLE)
	{
		t_vec3 a = obj->u_obj.tr.a;
		t_vec3 b = obj->u_obj.tr.b;
		t_vec3 c = obj->u_obj.tr.c;
		t_vec3 col = obj->u_obj.tr.color;
		if (!ti_add_formatted_label(ti, app, x + 12, y,
			"A=(%.2f, %.2f, %.2f) B=(%.2f, %.2f, %.2f)",
			a.x, a.y, a.z, b.x, b.y, b.z))
			return (0);
		if (!ti_add_formatted_label(ti, app, x + 12, y,
			"C=(%.2f, %.2f, %.2f) color=(%.2f, %.2f, %.2f)",
			c.x, c.y, c.z, col.x, col.y, col.z))
			return (0);
		if ((obj->u_obj.tr.has_checker || obj->u_obj.tr.has_bump)
			&& !ti_add_formatted_label(ti, app, x + 12, y,
				"checker=%s scale=%.2f bump=%s strength=%.2f",
				obj->u_obj.tr.has_checker ? "yes" : "no",
				obj->u_obj.tr.checker_scale,
				obj->u_obj.tr.has_bump ? "yes" : "no",
				obj->u_obj.tr.bump_strength))
			return (0);
	}
	else if (obj->type == OBJ_HPARABOLOID)
	{
		t_vec3 c = obj->u_obj.hp.center;
		t_vec3 axis = obj->u_obj.hp.axis;
		t_vec3 col = obj->u_obj.hp.color;
		if (!ti_add_formatted_label(ti, app, x + 12, y,
			"center=(%.2f, %.2f, %.2f) axis=(%.2f, %.2f, %.2f)",
			c.x, c.y, c.z, axis.x, axis.y, axis.z))
			return (0);
		if (!ti_add_formatted_label(ti, app, x + 12, y,
			"rx=%.2f ry=%.2f h=%.2f color=(%.2f, %.2f, %.2f)",
			obj->u_obj.hp.rx, obj->u_obj.hp.ry,
			obj->u_obj.hp.height, col.x, col.y, col.z))
			return (0);
		if ((obj->u_obj.hp.has_checker || obj->u_obj.hp.has_bump)
			&& !ti_add_formatted_label(ti, app, x + 12, y,
				"checker=%s scale=%.2f bump=%s strength=%.2f",
				obj->u_obj.hp.has_checker ? "yes" : "no",
				obj->u_obj.hp.checker_scale,
				obj->u_obj.hp.has_bump ? "yes" : "no",
				obj->u_obj.hp.bump_strength))
			return (0);
	}
	return (1);
}

void ti_update_info(t_app *app)
{
	t_toggle_info	*ti;
	int		x;
	int		y;
	size_t	object_count;
	size_t	index;
	const t_object	*obj;
	struct timeval now;
	double	uptime_ms;

	if (!app || !app->mlx)
		return;
	ti = &app->overlay;
	ti_clear_labels(ti);
	if (!ti->overlay)
		return;
	x = 16;
	y = 16;
	if (!ti_add_formatted_label(ti, app, x, &y, "Frames: %lu",
		app->stats.frame_count))
	{
		ti_clear_labels(ti);
		return;
	}
	if (!ti_add_formatted_label(ti, app, x, &y, "Render time: %.2f ms",
		app->stats.last_ms))
	{
		ti_clear_labels(ti);
		return;
	}
	if (!ti_add_formatted_label(ti, app, x, &y, "Average time: %.2f ms",
		(app->stats.frame_count > 0) ? app->stats.avg_ms : 0.0))
	{
		ti_clear_labels(ti);
		return;
	}
	if (app->stats.last_ms > 0.0)
	{
		if (!ti_add_formatted_label(ti, app, x, &y, "FPS: %.2f",
			app->stats.last_fps))
		{
			ti_clear_labels(ti);
			return;
		}
	}
	else if (!ti_add_formatted_label(ti, app, x, &y, "FPS: --"))
	{
		ti_clear_labels(ti);
		return;
	}
	if (!ti_add_formatted_label(ti, app, x, &y,
		"Accumulated render: %.2f ms", app->stats.total_ms))
	{
		ti_clear_labels(ti);
		return;
	}
	gettimeofday(&now, NULL);
	uptime_ms = ti_time_diff_ms(&app->stats.app_start, &now);
	if (!ti_add_formatted_label(ti, app, x, &y, "Uptime: %.2f s",
		uptime_ms / 1000.0))
	{
		ti_clear_labels(ti);
		return;
	}
	y += 10;
	if (app->scene.camera.present)
	{
		if (!ti_add_formatted_label(ti, app, x, &y,
			"Camera pos: (%.2f, %.2f, %.2f)",
			app->scene.camera.pos.x, app->scene.camera.pos.y,
			app->scene.camera.pos.z))
		{
			ti_clear_labels(ti);
			return;
		}
		if (!ti_add_formatted_label(ti, app, x, &y,
			"Camera dir: (%.2f, %.2f, %.2f)",
			app->scene.camera.dir.x, app->scene.camera.dir.y,
			app->scene.camera.dir.z))
		{
			ti_clear_labels(ti);
			return;
		}
		if (!ti_add_formatted_label(ti, app, x, &y,
			"Camera FOV: %.2f°", app->scene.camera.fov_deg))
		{
			ti_clear_labels(ti);
			return;
		}
	}
	else if (!ti_add_formatted_label(ti, app, x, &y,
		"Camera: not defined"))
	{
		ti_clear_labels(ti);
		return;
	}
	y += 10;
	if (app->scene.ambient.present)
	{
		if (!ti_add_formatted_label(ti, app, x, &y,
			"Ambient ratio=%.2f color=(%.2f, %.2f, %.2f)",
			app->scene.ambient.ratio, app->scene.ambient.color.x,
			app->scene.ambient.color.y, app->scene.ambient.color.z))
		{
			ti_clear_labels(ti);
			return;
		}
	}
	else if (!ti_add_formatted_label(ti, app, x, &y,
		"Ambient: not defined"))
	{
		ti_clear_labels(ti);
		return;
	}
	if (app->scene.light.present)
	{
		if (!ti_add_formatted_label(ti, app, x, &y,
			"Light pos=(%.2f, %.2f, %.2f) bright=%.2f color=(%.2f, %.2f, %.2f)",
			app->scene.light.pos.x, app->scene.light.pos.y,
			app->scene.light.pos.z, app->scene.light.bright,
			app->scene.light.color.x, app->scene.light.color.y,
			app->scene.light.color.z))
		{
			ti_clear_labels(ti);
			return;
		}
	}
	else if (!ti_add_formatted_label(ti, app, x, &y,
		"Light: not defined"))
	{
		ti_clear_labels(ti);
		return;
	}
	y += 10;
	object_count = 0;
	obj = app->scene.objects;
	while (obj)
	{
		object_count++;
		obj = obj->next;
	}
	if (!ti_add_formatted_label(ti, app, x, &y,
		"Objects: %zu", object_count))
	{
		ti_clear_labels(ti);
		return;
	}
	obj = app->scene.objects;
	index = 0;
	while (obj)
	{
		if (!ti_add_object_details(ti, app, x, &y, index, obj))
		{
			ti_clear_labels(ti);
			return;
		}
		index++;
		obj = obj->next;
	}
	ti_set_labels_visibility(ti, ti->visible);
}
