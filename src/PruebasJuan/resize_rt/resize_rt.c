#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <math.h>
#include "../../../libraries/libft/libft.h"
#include "../../../libraries/get_next_line/get_next_line.h"
#include "../../../include/vec3.h"

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

static int	parse_vec3(const char *token, t_vec3 *out)
{
	char	**parts;
	float	values[3];
	int		count;

	parts = ft_split(token, ',');
	if (!parts)
		return (0);
	count = 0;
	while (count < 3 && parts[count])
	{
		if (!parse_float_str(parts[count], &values[count]))
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
	*out = make_vec3(values[0], values[1], values[2]);
	ft_freearr(parts);
	return (1);
}

static void	scale_vec3(t_vec3 *v, float scale)
{
	v->x *= scale;
	v->y *= scale;
	v->z *= scale;
}

static int	process_triangle_line(const char *line, int fd_out, float scale)
{
	char	**parts;
	t_vec3	a;
	t_vec3	b;
	t_vec3	c;

	parts = ft_split(line, ' ');
	if (!parts)
		return (0);
	if (!parts[0] || !parts[1] || !parts[2] || !parts[3] || !parts[4])
	{
		ft_freearr(parts);
		return (0);
	}
	if (!parse_vec3(parts[1], &a)
		|| !parse_vec3(parts[2], &b)
		|| !parse_vec3(parts[3], &c))
	{
		ft_freearr(parts);
		return (0);
	}
	scale_vec3(&a, scale);
	scale_vec3(&b, scale);
	scale_vec3(&c, scale);
	write(fd_out, "tr ", 3);
	write_vec3(fd_out, a);
	write(fd_out, " ", 1);
	write_vec3(fd_out, b);
	write(fd_out, " ", 1);
	write_vec3(fd_out, c);
	write(fd_out, " ", 1);
	write(fd_out, parts[4], ft_strlen(parts[4]));
	write(fd_out, "\n", 1);
	ft_freearr(parts);
	return (1);
}

static int	process_bounds_line(const char *line, int fd_out, float scale,
		const char *prefix)
{
	t_vec3	vec;
	const char	*vec_str;
	size_t	prefix_len;

	prefix_len = ft_strlen(prefix);
	if (ft_strncmp(line, prefix, prefix_len) != 0)
		return (0);
	vec_str = line + prefix_len;
	while (*vec_str == ' ')
		vec_str++;
	if (!parse_vec3(vec_str, &vec))
		return (0);
	scale_vec3(&vec, scale);
	write(fd_out, prefix, prefix_len);
	write_vec3(fd_out, vec);
	write(fd_out, "\n", 1);
	return (1);
}

static void	write_raw_line(int fd, const char *line, bool add_newline)
{
	size_t	len;

	len = ft_strlen(line);
	if (len > 0)
		write(fd, line, len);
	if (add_newline)
		write(fd, "\n", 1);
}

static int	process_line(const char *line, bool had_newline, int fd_out,
		float scale)
{
	if (ft_strncmp(line, "tr ", 3) == 0)
	{
		if (!process_triangle_line(line, fd_out, scale))
			return (0);
		return (1);
	}
	if (ft_strncmp(line, "# Bounds min ", 13) == 0)
		return (process_bounds_line(line, fd_out, scale, "# Bounds min "));
	if (ft_strncmp(line, "# Bounds max ", 13) == 0)
		return (process_bounds_line(line, fd_out, scale, "# Bounds max "));
	write_raw_line(fd_out, line, had_newline);
	return (1);
}

static int	scale_file(const char *in_path, const char *out_path, float scale)
{
	int		fd_in;
	int		fd_out;
	char	*line;
	bool	had_newline;
	size_t	len;

	fd_in = open(in_path, O_RDONLY);
	if (fd_in < 0)
		return (0);
	fd_out = open(out_path, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if (fd_out < 0)
	{
		close(fd_in);
		return (0);
	}
	line = get_next_line(fd_in);
	while (line)
	{
		had_newline = false;
		len = ft_strlen(line);
		while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
		{
			if (line[len - 1] == '\n')
				had_newline = true;
			line[len - 1] = '\0';
			len--;
		}
		if (*line)
		{
			if (!process_line(line, had_newline, fd_out, scale))
			{
				free(line);
				close(fd_in);
				close(fd_out);
				return (0);
			}
		}
		else if (had_newline)
			write(fd_out, "\n", 1);
		free(line);
		line = get_next_line(fd_in);
	}
	close(fd_in);
	close(fd_out);
	return (1);
}

int	main(int ac, char **av)
{
	float	scale;

	if (ac != 4)
		return (error_msg("usage: ./resize_rt input.rt output.rt scale"));
	if (!parse_float_str(av[3], &scale))
		return (error_msg("invalid scale factor"));
	if (!scale_file(av[1], av[2], scale))
		return (error_msg("failed to resize scene"));
	return (0);
}
