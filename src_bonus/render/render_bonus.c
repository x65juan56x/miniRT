#include <float.h>
#include <pthread.h>
#include <stdlib.h>
#include "../../include_bonus/render_bonus.h"
#include "../../include_bonus/camera_bonus.h"
#include "../../include_bonus/app_bonus.h"
#include "../../include_bonus/shading_bonus.h"

#define MAX_RENDER_THREADS 16

typedef struct s_render_task
{
	const t_scene		*scene;
	const t_cam_frame	*frame;
	uint32_t			*framebuffer;
	uint32_t			width;
	uint32_t			height;
	int					show_normals;
	int					y_start;
	int					y_end;
}	t_render_task;

static t_scene	*scene_clone(const t_scene *src)
{
	t_scene		*clone;
	t_object	*it;
	t_object	*tail;

	clone = (t_scene *)malloc(sizeof(t_scene));
	if (!clone)
		return (NULL);
	*clone = *src;
	clone->objects = NULL;
	tail = NULL;
	it = src->objects;
	while (it)
	{
		t_object	*node;

		node = (t_object *)malloc(sizeof(t_object));
		if (!node)
		{
			while (clone->objects)
			{
				t_object	*next;

				next = clone->objects->next;
				free(clone->objects);
				clone->objects = next;
			}
			free(clone);
			return (NULL);
		}
		node->type = it->type;
		node->u_obj = it->u_obj;
		node->next = NULL;
		if (!clone->objects)
		{
			clone->objects = node;
			tail = node;
		}
		else
		{
			tail->next = node;
			tail = node;
		}
		it = it->next;
	}
	return (clone);
}

static void	scene_clone_free(t_scene *scene)
{
	t_object	*it;
	t_object	*next;

	if (!scene)
		return ;
	it = scene->objects;
	while (it)
	{
		next = it->next;
		free(it);
		it = next;
	}
	free(scene);
}

static t_vec3	trace_pixel(const t_scene *scene, t_ray r, int show_normals)
{
	t_hit	hit;

	scene_hit(scene, r, FLT_MAX, &hit);
	if (show_normals && hit.ok)
		return (v3_mul(v3_add(hit.n, v3(1.0f, 1.0f, 1.0f)), 0.5f));
	return (shade_lambert(scene, &hit));
}

static void	*render_worker(void *param)
{
	t_render_task	*task;
	t_render_aux	vars;
	uint32_t		x;
	int			y;
	float			width_f;
	float			height_f;

	task = (t_render_task *)param;
	vars.frame = *task->frame;
	width_f = (float)task->width;
	height_f = (float)task->height;
	y = task->y_start - 1;
	while (++y < task->y_end)
	{
		vars.v = 1.0f - (((float)y + 0.5f) / height_f);
		x = 0;
		while (x < task->width)
		{
			vars.u = ((float)x + 0.5f) / width_f;
			vars.sample = v3_add(vars.frame.lower_left,
				v3_add(v3_mul(vars.frame.horizontal, vars.u),
					v3_mul(vars.frame.vertical, vars.v)));
			vars.dir = v3_norm(v3_sub(vars.sample, vars.frame.origin));
			task->framebuffer[(size_t)y * (size_t)task->width + x]
				= vec3_to_rgba(trace_pixel(task->scene,
					ray(vars.frame.origin, vars.dir), task->show_normals));
			++x;
		}
	}
	return (NULL);
}

static void	render_scene_single_thread(t_app *app, const t_cam_frame *frame)
{
	t_render_task	task;

	task.scene = &app->scene;
	task.frame = frame;
	task.framebuffer = app->framebuffer;
	task.width = app->image->width;
	task.height = app->image->height;
	task.show_normals = app->show_normals;
	task.y_start = 0;
	task.y_end = (int)app->image->height;
	(void)render_worker(&task);
}

static int	clamp_thread_count(const t_app *app)
{
	int	count;
	uint32_t	height;

	count = app->thread_count;
	if (count < 1)
		count = 1;
	if (count > MAX_RENDER_THREADS)
		count = MAX_RENDER_THREADS;
	height = app->image->height;
	if (height == 0)
		return (1);
	if ((uint32_t)count > height)
		count = (int)height;
	return (count);
}

static int	alloc_workers(pthread_t **threads, t_render_task **tasks, int thread_count)
{
	int	spawn_count;

	spawn_count = thread_count - 1;
	*threads = NULL;
	if (spawn_count > 0)
	{
		*threads = (pthread_t *)malloc(sizeof(pthread_t) * (size_t)spawn_count);
		if (!*threads)
			return (0);
	}
	*tasks = (t_render_task *)malloc(sizeof(t_render_task) * (size_t)thread_count);
	if (!*tasks)
	{
		free(*threads);
		*threads = NULL;
		return (0);
	}
	return (1);
}

static void	fill_tasks(t_render_task *tasks, t_scene **scene_clones,
		const t_app *app, const t_cam_frame *frame, int thread_count)
{
	int	rows_per_thread;
	int	remainder;
	int	i;
	int	y;

	rows_per_thread = (int)app->image->height / thread_count;
	remainder = (int)app->image->height % thread_count;
	i = 0;
	y = 0;
	while (i < thread_count)
	{
		int	row_count;

		row_count = rows_per_thread + (i < remainder);
		tasks[i].scene = scene_clones[i];
		tasks[i].frame = frame;
		tasks[i].framebuffer = app->framebuffer;
		tasks[i].width = app->image->width;
		tasks[i].height = app->image->height;
		tasks[i].show_normals = app->show_normals;
		tasks[i].y_start = y;
		y += row_count;
		tasks[i].y_end = y;
		++i;
	}
}

static int	launch_workers(pthread_t *threads, t_render_task *tasks, int spawn_count)
{
	int	i;

	i = 0;
	while (i < spawn_count)
	{
		if (pthread_create(&threads[i], NULL, render_worker, &tasks[i]) != 0)
		{
			int	j;

			j = i;
			while (j-- > 0)
				pthread_join(threads[j], NULL);
			return (0);
		}
		++i;
	}
	return (1);
}

static void	join_workers(pthread_t *threads, int spawn_count)
{
	int	i;

	i = 0;
	while (i < spawn_count)
	{
		pthread_join(threads[i], NULL);
		++i;
	}
}

static void	free_scene_clones(t_scene **clones, int count)
{
	int	index;

	if (!clones)
		return ;
	index = 0;
	while (index < count)
	{
		scene_clone_free(clones[index]);
		++index;
	}
	free(clones);
}

void	render_scene(t_app *app)
{
	t_cam_frame		frame;
	t_render_task	*tasks;
	pthread_t		*threads;
	t_scene			**scene_clones;
	int			thread_count;
	int			spawn_count;

	camera_build_frame(&app->scene.camera, app->image->width,
		app->image->height, &frame);
	thread_count = clamp_thread_count(app);
	if (thread_count <= 1)
	{
		render_scene_single_thread(app, &frame);
		return;
	}
	spawn_count = thread_count - 1;
	if (!alloc_workers(&threads, &tasks, thread_count))
	{
		render_scene_single_thread(app, &frame);
		return;
	}
	scene_clones = (t_scene **)malloc(sizeof(t_scene *) * (size_t)thread_count);
	if (!scene_clones)
	{
		free(tasks);
		free(threads);
		render_scene_single_thread(app, &frame);
		return;
	}
	{
		int	idx;

		idx = 0;
		while (idx < thread_count)
		{
			scene_clones[idx] = scene_clone(&app->scene);
			if (!scene_clones[idx])
			{
				while (idx-- > 0)
					scene_clone_free(scene_clones[idx]);
				free(scene_clones);
				free(tasks);
				free(threads);
				render_scene_single_thread(app, &frame);
				return;
			}
			++idx;
		}
	}
	fill_tasks(tasks, scene_clones, app, &frame, thread_count);
	if (!launch_workers(threads, tasks, spawn_count))
	{
		free_scene_clones(scene_clones, thread_count);
		free(tasks);
		free(threads);
		render_scene_single_thread(app, &frame);
		return;
	}
	(void)render_worker(&tasks[thread_count - 1]);
	join_workers(threads, spawn_count);
	free_scene_clones(scene_clones, thread_count);
	free(threads);
	free(tasks);
}
