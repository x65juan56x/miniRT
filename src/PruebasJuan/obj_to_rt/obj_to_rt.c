#include "obj_to_rt.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>
#include "../../../libraries/libft/libft.h"
#include "../../../libraries/get_next_line/get_next_line.h"

static t_vec3	make_vec3(float x, float y, float z)
{
    t_vec3	v;

    v.x = x;
    v.y = y;
    v.z = z;
    return (v);
}

static int	parse_float_str(const char *s, float *out)
{
    char	*end;
    float	value;

    if (!s)
        return (0);
    errno = 0;
    value = strtof(s, &end);
    if (errno == ERANGE || end == s || *end != '\0')
        return (0);
    *out = value;
    return (1);
}

static int	error_msg(const char *msg)
{
    write(2, "Error: ", 7);
    write(2, msg, ft_strlen(msg));
    write(2, "\n", 1);
    return (1);
}

static void	free_mesh(t_mesh *mesh)
{
    free(mesh->vertices);
    free(mesh->normals);
    free(mesh->texcoords);
    free(mesh->faces);
    ft_memset(mesh, 0, sizeof(*mesh));
}

static int	grow_vertices(t_mesh *mesh)
{
    size_t	new_cap;
    t_vec3	*new_data;

    new_cap = 8;
    if (mesh->v_cap > 0)
        new_cap = mesh->v_cap * 2;
    new_data = (t_vec3 *)malloc(sizeof(t_vec3) * new_cap);
    if (!new_data)
        return (0);
    if (mesh->vertices)
        ft_memcpy(new_data, mesh->vertices,
            sizeof(t_vec3) * mesh->v_count);
    free(mesh->vertices);
    mesh->vertices = new_data;
    mesh->v_cap = new_cap;
    return (1);
}

static int	grow_faces(t_mesh *mesh)
{
    size_t	new_cap;
    t_face	*new_data;

    new_cap = 8;
    if (mesh->f_cap > 0)
        new_cap = mesh->f_cap * 2;
    new_data = (t_face *)malloc(sizeof(t_face) * new_cap);
    if (!new_data)
        return (0);
    if (mesh->faces)
        ft_memcpy(new_data, mesh->faces,
            sizeof(t_face) * mesh->f_count);
    free(mesh->faces);
    mesh->faces = new_data;
    mesh->f_cap = new_cap;
    return (1);
}

static int	grow_normals(t_mesh *mesh)
{
    size_t	new_cap;
    t_vec3	*new_data;

    new_cap = 8;
    if (mesh->vn_cap > 0)
        new_cap = mesh->vn_cap * 2;
    new_data = (t_vec3 *)malloc(sizeof(t_vec3) * new_cap);
    if (!new_data)
        return (0);
    if (mesh->normals)
        ft_memcpy(new_data, mesh->normals,
            sizeof(t_vec3) * mesh->vn_count);
    free(mesh->normals);
    mesh->normals = new_data;
    mesh->vn_cap = new_cap;
    return (1);
}

static int	grow_texcoords(t_mesh *mesh)
{
    size_t	new_cap;
    t_vec3	*new_data;

    new_cap = 8;
    if (mesh->vt_cap > 0)
        new_cap = mesh->vt_cap * 2;
    new_data = (t_vec3 *)malloc(sizeof(t_vec3) * new_cap);
    if (!new_data)
        return (0);
    if (mesh->texcoords)
        ft_memcpy(new_data, mesh->texcoords,
            sizeof(t_vec3) * mesh->vt_count);
    free(mesh->texcoords);
    mesh->texcoords = new_data;
    mesh->vt_cap = new_cap;
    return (1);
}

static int	append_vertex(t_mesh *mesh, t_vec3 v)
{
    if (mesh->v_count == mesh->v_cap)
        if (!grow_vertices(mesh))
            return (0);
    mesh->vertices[mesh->v_count] = v;
    mesh->v_count++;
    return (1);
}

static int	append_normal(t_mesh *mesh, t_vec3 n)
{
    if (mesh->vn_count == mesh->vn_cap)
        if (!grow_normals(mesh))
            return (0);
    mesh->normals[mesh->vn_count] = n;
    mesh->vn_count++;
    return (1);
}

static int	append_texcoord(t_mesh *mesh, t_vec3 t)
{
    if (mesh->vt_count == mesh->vt_cap)
        if (!grow_texcoords(mesh))
            return (0);
    mesh->texcoords[mesh->vt_count] = t;
    mesh->vt_count++;
    return (1);
}

static int	append_face(t_mesh *mesh, const int v[3], const int vt[3],
            const int vn[3])
{
    if (mesh->f_count == mesh->f_cap)
        if (!grow_faces(mesh))
            return (0);
    ft_memcpy(mesh->faces[mesh->f_count].v, v, sizeof(int) * 3);
    ft_memcpy(mesh->faces[mesh->f_count].vt, vt, sizeof(int) * 3);
    ft_memcpy(mesh->faces[mesh->f_count].vn, vn, sizeof(int) * 3);
    mesh->f_count++;
    return (1);
}

static void	trim_line(char *line)
{
    size_t	i;

    i = 0;
    while (line[i])
    {
        if (line[i] == '\n' || line[i] == '\r')
            line[i] = '\0';
        i++;
    }
}

static int	parse_obj_index(const char *str, size_t count, int *out)
{
    long	idx;

    if (!str || str[0] == '\0')
    {
        *out = -1;
        return (1);
    }
    idx = (long)ft_atoi(str);
    if (idx == 0)
        return (0);
    if (idx < 0)
        idx = (long)count + idx;
    else
        idx -= 1;
    if (idx < 0 || (size_t)idx >= count)
        return (0);
    *out = (int)idx;
    return (1);
}

static int	parse_face_token(char *token, t_mesh *mesh, int *v, int *vt, int *vn)
{
    char	*first;
    char	*second;
    char	*vt_str;
    char	*vn_str;

    *vt = -1;
    *vn = -1;
    first = ft_strchr(token, '/');
    if (!parse_obj_index(token, mesh->v_count, v))
        return (0);
    if (!first)
        return (1);
    vt_str = first + 1;
    second = ft_strchr(vt_str, '/');
    if (second)
    {
        vn_str = second + 1;
        *second = '\0';
    }
    else
        vn_str = NULL;
    *first = '\0';
    if (vt_str && !parse_obj_index(vt_str, mesh->vt_count, vt))
        return (0);
    if (vn_str && !parse_obj_index(vn_str, mesh->vn_count, vn))
        return (0);
    return (1);
}

static int	parse_vertex_line(char *line, t_mesh *mesh)
{
    char	**parts;
    float	val[3];
    int		i;

    parts = ft_split(line + 2, ' ');
    if (!parts)
        return (0);
    i = 0;
    while (i < 3 && parts[i])
    {
        if (!parse_float_str(parts[i], &val[i]))
        {
            ft_freearr(parts);
            return (0);
        }
        i++;
    }
    if (i != 3)
    {
        ft_freearr(parts);
        return (0);
    }
    if (!append_vertex(mesh, make_vec3(val[0], val[1], val[2])))
    {
        ft_freearr(parts);
        return (0);
    }
    ft_freearr(parts);
    return (1);
}

static int	parse_texcoord_line(char *line, t_mesh *mesh)
{
    char	**parts;
    float	uv[3];
    float	w;
    int		count;

    parts = ft_split(line + 3, ' ');
    if (!parts)
        return (0);
    count = 0;
    while (count < 3 && parts[count])
    {
        if (!parse_float_str(parts[count], &uv[count]))
        {
            ft_freearr(parts);
            return (0);
        }
        count++;
    }
    if (count < 2)
    {
        ft_freearr(parts);
        return (0);
    }
    w = 0.0f;
    if (count > 2)
        w = uv[2];
    if (!append_texcoord(mesh, make_vec3(uv[0], uv[1], w)))
    {
        ft_freearr(parts);
        return (0);
    }
    ft_freearr(parts);
    return (1);
}

static int	parse_normal_line(char *line, t_mesh *mesh)
{
    char	**parts;
    float	val[3];
    int		count;

    parts = ft_split(line + 3, ' ');
    if (!parts)
        return (0);
    count = 0;
    while (count < 3 && parts[count])
    {
        if (!parse_float_str(parts[count], &val[count]))
        {
            ft_freearr(parts);
            return (0);
        }
        count++;
    }
    if (count != 3)
    {
        ft_freearr(parts);
        return (0);
    }
    if (!append_normal(mesh, make_vec3(val[0], val[1], val[2])))
    {
        ft_freearr(parts);
        return (0);
    }
    ft_freearr(parts);
    return (1);
}

static int	parse_face_line(char *line, t_mesh *mesh)
{
    char	**parts;
    int		v[3];
    int		vt[3];
    int		vn[3];
    int		i;

    parts = ft_split(line + 2, ' ');
    if (!parts)
        return (0);
    i = 0;
    while (i < 3 && parts[i])
    {
        if (!parse_face_token(parts[i], mesh, &v[i], &vt[i], &vn[i]))
        {
            ft_freearr(parts);
            return (0);
        }
        i++;
    }
    ft_freearr(parts);
    if (i != 3)
        return (0);
    return (append_face(mesh, v, vt, vn));
}

static int	handle_line(char *line, t_mesh *mesh)
{
	if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
		return (parse_texcoord_line(line, mesh));
	if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
		return (parse_normal_line(line, mesh));
    if (line[0] == 'v' && line[1] == ' ')
        return (parse_vertex_line(line, mesh));
    if (line[0] == 'f' && line[1] == ' ')
        return (parse_face_line(line, mesh));
    return (1);
}

static int	read_obj_file(const char *path, t_mesh *mesh)
{
    int		fd;
    char	*line;
    int		ok;

    fd = open(path, O_RDONLY);
    if (fd < 0)
        return (0);
    ok = 1;
    line = get_next_line(fd);
    while (line && ok)
    {
        trim_line(line);
        if (line[0] != '\0')
            ok = handle_line(line, mesh);
        free(line);
        if (ok)
            line = get_next_line(fd);
        else
            line = NULL;
    }
    if (line)
        free(line);
    close(fd);
    return (ok);
}

static void	write_digits(int fd, long value)
{
    char	c;

    if (value >= 10)
        write_digits(fd, value / 10);
    c = (char)('0' + (value % 10));
    write(fd, &c, 1);
}

static void	write_fraction(int fd, long frac)
{
    char	buf[6];
    int		i;

    i = 5;
    while (i >= 0)
    {
        buf[i] = (char)('0' + (frac % 10));
        frac /= 10;
        i--;
    }
    write(fd, buf, 6);
}

static void	write_float(int fd, float value)
{
    long	whole;
    long	frac;
    float	mod;

    if (value < 0.0f)
    {
        write(fd, "-", 1);
        value = -value;
    }
    whole = (long)value;
    mod = value - (float)whole;
    frac = (long)roundf(mod * 1000000.0f);
    if (frac == 1000000)
    {
        whole += 1;
        frac = 0;
    }
    write_digits(fd, whole);
    write(fd, ".", 1);
    write_fraction(fd, frac);
}

static void	write_vec3(int fd, t_vec3 v)
{
    write_float(fd, v.x);
    write(fd, ",", 1);
    write_float(fd, v.y);
    write(fd, ",", 1);
    write_float(fd, v.z);
}

static void	write_color(int fd, int idx)
{
    (void)idx;
    write(fd, "200,200,200", 11);
}

static void	write_triangle_entry(int fd, t_vec3 a, t_vec3 b, t_vec3 c, int idx)
{
    write(fd, "tr ", 3);
    write_vec3(fd, a);
    write(fd, " ", 1);
    write_vec3(fd, b);
    write(fd, " ", 1);
    write_vec3(fd, c);
    write(fd, " ", 1);
    write_color(fd, idx);
    write(fd, "\n", 1);
}

static void	center_mesh(t_mesh *mesh, t_vec3 *out_min, t_vec3 *out_max)
{
    size_t	i;
    t_vec3	min;
    t_vec3	max;
    t_vec3	shift;

	if (mesh->v_count == 0)
    {
        if (out_min)
            *out_min = make_vec3(0.0f, 0.0f, 0.0f);
        if (out_max)
            *out_max = make_vec3(0.0f, 0.0f, 0.0f);
        return ;
    }
    min = mesh->vertices[0];
    max = mesh->vertices[0];
    i = 1;
    while (i < mesh->v_count)
    {
        if (mesh->vertices[i].x < min.x)
            min.x = mesh->vertices[i].x;
        if (mesh->vertices[i].y < min.y)
            min.y = mesh->vertices[i].y;
        if (mesh->vertices[i].z < min.z)
            min.z = mesh->vertices[i].z;
        if (mesh->vertices[i].x > max.x)
            max.x = mesh->vertices[i].x;
        if (mesh->vertices[i].y > max.y)
            max.y = mesh->vertices[i].y;
        if (mesh->vertices[i].z > max.z)
            max.z = mesh->vertices[i].z;
        i++;
    }
    shift.x = (min.x + max.x) * 0.5f;
    shift.y = (min.y + max.y) * 0.5f;
    shift.z = (min.z + max.z) * 0.5f;
    i = 0;
    while (i < mesh->v_count)
    {
        mesh->vertices[i].x -= shift.x;
        mesh->vertices[i].y -= shift.y;
        mesh->vertices[i].z -= shift.z;
        i++;
    }
    if (out_min)
    {
        out_min->x = min.x - shift.x;
        out_min->y = min.y - shift.y;
        out_min->z = min.z - shift.z;
    }
    if (out_max)
    {
        out_max->x = max.x - shift.x;
        out_max->y = max.y - shift.y;
        out_max->z = max.z - shift.z;
    }
}

static void	write_faces_as_triangles(int fd, const t_mesh *mesh)
{
    size_t	i;
    t_vec3	a;
    t_vec3	b;
    t_vec3	c;

    i = 0;
    while (i < mesh->f_count)
    {
        a = mesh->vertices[mesh->faces[i].v[0]];
        b = mesh->vertices[mesh->faces[i].v[1]];
        c = mesh->vertices[mesh->faces[i].v[2]];
        write_triangle_entry(fd, a, b, c, (int)i);
        i++;
    }
}

static int	write_header(int fd)
{
    if (write(fd, "A 0.1 255,255,255\n", ft_strlen("A 0.1 255,255,255\n")) < 0)
        return (0);
    if (write(fd, "C 0,0,5 0,0,-1 60\n", ft_strlen("C 0,0,5 0,0,-1 60\n")) < 0)
        return (0);
    if (write(fd, "L 10,10,10 0.7 255,255,255\n", ft_strlen("L 10,10,10 0.7 255,255,255\n")) < 0)
        return (0);
    return (1);
}

static void	write_bounds(int fd, t_vec3 min, t_vec3 max)
{
    write(fd, "# Bounds min ", sizeof("# Bounds min ") - 1);
    write_vec3(fd, min);
    write(fd, "\n", 1);
    write(fd, "# Bounds max ", sizeof("# Bounds max ") - 1);
    write_vec3(fd, max);
    write(fd, "\n", 1);
}

static int	write_rt(const char *path, const t_mesh *mesh, t_vec3 min, t_vec3 max)
{
    int	fd;

    fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd < 0)
        return (0);
    if (!write_header(fd))
    {
        close(fd);
        return (0);
    }
    write_faces_as_triangles(fd, mesh);
    write_bounds(fd, min, max);
    close(fd);
    return (1);
}

int	main(int ac, char **av)
{
    t_mesh	mesh;
    t_vec3	bbox_min;
    t_vec3	bbox_max;

    if (ac != 3)
        return (error_msg("usage: ./obj_to_rt input.obj output.rt"));
    ft_memset(&mesh, 0, sizeof(mesh));
    if (!read_obj_file(av[1], &mesh))
    {
        free_mesh(&mesh);
        return (error_msg("failed to load .obj"));
    }
    center_mesh(&mesh, &bbox_min, &bbox_max);
    if (!write_rt(av[2], &mesh, bbox_min, bbox_max))
    {
        free_mesh(&mesh);
        return (error_msg("failed to write .rt"));
    }
    free_mesh(&mesh);
    return (0);
}
