#include <float.h>
#include <math.h>
#include "../../include_bonus/render_bonus.h"
#include "../../include_bonus/camera_bonus.h"
#include "../../include_bonus/app_bonus.h"
#include "../../include_bonus/shading_bonus.h"

static float	clamp01(float v)
{
	if (v < 0.0f)
		return (0.0f);
	if (v > 1.0f)
		return (1.0f);
	return (v);
}

static t_vec3	trace_recursive(const t_scene *scene, t_ray r, int depth);

static t_vec3	shade_surface(const t_scene *scene, const t_hit *hit,
		t_ray incoming, int depth)
{
	float	kr;
	t_vec3	local;
	t_vec3	ref_dir;
	float	dir_len2;
	t_vec3	reflect_col;

	local = shade_lambert(scene, hit);
	kr = clamp01(hit->reflectivity);
	if (depth <= 0 || kr <= 0.0f)
		return (local);
	ref_dir = v3_sub(incoming.dir,
			v3_mul(hit->n, 2.0f * v3_dot(incoming.dir, hit->n)));
	dir_len2 = v3_len2(ref_dir);
	if (dir_len2 < 1e-10f)
		return (local);
	ref_dir = v3_mul(ref_dir, 1.0f / sqrtf(dir_len2));
	reflect_col = trace_recursive(scene,
			ray(v3_add(hit->p, v3_mul(hit->n, fmaxf(1e-4f, 1e-3f * hit->t))),
				ref_dir), depth - 1);
	return (v3_add(v3_mul(local, 1.0f - kr),
			v3_mul(reflect_col, kr)));
}

static t_vec3	trace_recursive(const t_scene *scene, t_ray r, int depth)
{
	t_hit	hit;

	if (!scene_hit(scene, r, FLT_MAX, &hit) || !hit.ok)
		return (v3(0.0f, 0.0f, 0.0f));
	return (shade_surface(scene, &hit, r, depth));
}

static t_vec3	trace_pixel(const t_scene *scene, t_ray r, int show_normals)
{
	t_hit	hit;

	if (!scene_hit(scene, r, FLT_MAX, &hit) || !hit.ok)
		return (v3(0.0f, 0.0f, 0.0f));
	if (show_normals)
		return (v3_mul(v3_add(hit.n, v3(1.0f, 1.0f, 1.0f)), 0.5f));
	return (shade_surface(scene, &hit, r, scene->reflection_depth));
}

void	render_scene(t_app *app)
{
	int				x;
	int				y;
	t_render_aux	vars;

	camera_build_frame(&app->scene.camera, app->image->width,
		app->image->height, &vars.frame);
	y = -1;
	while ((uint32_t)++y < app->image->height)
	{
		x = -1;
		while ((uint32_t)++x < app->image->width)
		{
			vars.u = ((float)x + 0.5f) / (float)app->image->width;
			vars.v = 1.0f - (((float)y + 0.5f) / (float)app->image->height);
			vars.sample = v3_add(vars.frame.lower_left,
					v3_add(v3_mul(vars.frame.horizontal, vars.u),
						v3_mul(vars.frame.vertical, vars.v)));
			vars.dir = v3_norm(v3_sub(vars.sample, vars.frame.origin));
			app->framebuffer[y * app->image->width + x]
				= vec3_to_rgba(trace_pixel(&app->scene,
						ray(vars.frame.origin, vars.dir), app->show_normals));
		}
	}
}

/*
static t_vec3	shade_surface(const t_scene *scene, const t_hit *hit,
		t_ray incoming, int depth)
{
	float	kr;
	t_vec3	local;
	t_vec3	ref_dir;
	float	dir_len2;
	float	bias;
	t_vec3	reflect_col;
	float	mix_local;
	float	mix_reflect;

	local = shade_lambert(scene, hit);
	kr = clamp01(hit->reflectivity);
	if (depth <= 0 || kr <= 0.0f)
		return (local);
	ref_dir = v3_sub(incoming.dir,
		v3_mul(hit->n, 2.0f * v3_dot(incoming.dir, hit->n)));
	dir_len2 = v3_len2(ref_dir);
	if (dir_len2 < 1e-10f)
		return (local);
	ref_dir = v3_mul(ref_dir, 1.0f / sqrtf(dir_len2));
	bias = fmaxf(1e-4f, 1e-3f * hit->t);
	reflect_col = trace_recursive(scene,
		ray(v3_add(hit->p, v3_mul(hit->n, bias)), ref_dir), depth - 1);
	mix_local = 1.0f - kr;
	mix_reflect = kr;
	return (v3_add(v3_mul(local, mix_local),
		v3_mul(reflect_col, mix_reflect)));
}
*/