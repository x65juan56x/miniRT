#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "../../libraries/libft/libft.h"
#include "../../libraries/get_next_line/get_next_line.h"

typedef struct s_vec3
{
    float	x;
    float	y;
    float	z;
}	t_vec3;

static void	print_error(const char *msg)
{
    write(2, msg, ft_strlen(msg));
    write(2, "\n", 1);
}

static int	is_space(char c)
{
    return (c == ' ' || c == '\t');
}

static const char	*skip_spaces(const char *s)
{
    while (*s && is_space(*s))
        s++;
    return (s);
}

static int	parse_float_value(const char *str, float *out)
{
    char	*end;
    float	value;

    errno = 0;
    value = strtof(str, &end);
    if (end == str || errno != 0)
        return (0);
    if (*skip_spaces(end) != '\0')
        return (0);
    *out = value;
    return (1);
}

static int	parse_vector(const char *str, t_vec3 *out)
{
    char	*end;
    float	values[3];
    int		i;

    i = 0;
    while (i < 3)
    {
        errno = 0;
        values[i] = strtof(str, &end);
        if (end == str || errno != 0)
            return (0);
        if (i < 2)
        {
            if (*end != ',')
                return (0);
            str = end + 1;
        }
        else
            str = end;
        i++;
    }
    if (*skip_spaces(str) != '\0')
        return (0);
    out->x = values[0];
    out->y = values[1];
    out->z = values[2];
    return (1);
}

static char	*format_vector(t_vec3 v)
{
    char	buffer[128];
    int		len;
    char	*res;

    len = snprintf(buffer, sizeof(buffer), "%.6f,%.6f,%.6f", v.x, v.y, v.z);
    if (len < 0)
        return (NULL);
    res = (char *)malloc(len + 1);
    if (!res)
        return (NULL);
    ft_strlcpy(res, buffer, len + 1);
    return (res);
}

static void	free_tokens_array(char **tokens)
{
    int	idx;

    if (!tokens)
        return ;
    idx = 0;
    while (tokens[idx])
    {
        free(tokens[idx]);
        idx++;
    }
    free(tokens);
}

static int	count_tokens(const char *s)
{
    int	count;

    count = 0;
    s = skip_spaces(s);
    while (*s)
    {
        count++;
        while (*s && !is_space(*s))
        {
            if (*s == '\n')
                return (count);
            s++;
        }
        while (*s && is_space(*s))
            s++;
    }
    return (count);
}

static char	**split_tokens(const char *s)
{
    char	**array;
    int		count;
    int		start;
    int		idx;

    count = count_tokens(s);
    array = (char **)malloc(sizeof(char *) * (count + 1));
    if (!array)
        return (NULL);
    idx = 0;
    s = skip_spaces(s);
    while (*s && idx < count)
    {
        start = 0;
        while (s[start] && !is_space(s[start]) && s[start] != '\n')
            start++;
        array[idx] = ft_substr(s, 0, start);
        if (!array[idx])
        {
            free_tokens_array(array);
            return (NULL);
        }
        s += start;
        while (*s && is_space(*s))
            s++;
        idx++;
    }
    array[idx] = NULL;
    return (array);
}

static char	*join_tokens(char **tokens)
{
    size_t	total;
    char	*res;
    int		i;

    total = 0;
    i = 0;
    while (tokens[i])
    {
        total += ft_strlen(tokens[i]);
        if (tokens[i + 1])
            total++;
        i++;
    }
    res = (char *)malloc(total + 1);
    if (!res)
        return (NULL);
    res[0] = '\0';
    i = 0;
    while (tokens[i])
    {
        ft_strlcat(res, tokens[i], total + 1);
        if (tokens[i + 1])
            ft_strlcat(res, " ", total + 1);
        i++;
    }
    return (res);
}

static int	replace_vector(char **token, t_vec3 delta)
{
    t_vec3	vec;
    char	*new_str;

    if (!token || !*token)
        return (0);
    if (!parse_vector(*token, &vec))
        return (0);
    vec.x += delta.x;
    vec.y += delta.y;
    vec.z += delta.z;
    new_str = format_vector(vec);
    if (!new_str)
        return (0);
    free(*token);
    *token = new_str;
    return (1);
}
static int	translate_tokens(char **tokens, t_vec3 delta)
{
    if (!tokens || !tokens[0])
        return (1);
    if (ft_strcmp(tokens[0], "C") == 0)
        return (replace_vector(&tokens[1], delta));
    if (ft_strcmp(tokens[0], "L") == 0)
        return (replace_vector(&tokens[1], delta));
    if (ft_strcmp(tokens[0], "sp") == 0)
        return (replace_vector(&tokens[1], delta));
    if (ft_strcmp(tokens[0], "pl") == 0)
        return (replace_vector(&tokens[1], delta));
    if (ft_strcmp(tokens[0], "cy") == 0)
        return (replace_vector(&tokens[1], delta));
    if (ft_strcmp(tokens[0], "hp") == 0)
        return (replace_vector(&tokens[1], delta));
    if (ft_strcmp(tokens[0], "tr") == 0)
        return (replace_vector(&tokens[1], delta)
            && replace_vector(&tokens[2], delta)
            && replace_vector(&tokens[3], delta));
    return (1);
}

static int	process_line(int fd, char *line, int had_nl, t_vec3 delta)
{
    char	**tokens;
    char	*joined;
    const char	*trim;

    trim = skip_spaces(line);
    if (*trim == '\0' || *trim == '#')
    {
        write(fd, line, ft_strlen(line));
        if (had_nl)
            write(fd, "\n", 1);
        return (1);
    }
    tokens = split_tokens(line);
    if (!tokens)
        return (0);
    if (!translate_tokens(tokens, delta))
    {
        free_tokens_array(tokens);
        return (0);
    }
    joined = join_tokens(tokens);
    free_tokens_array(tokens);
    if (!joined)
        return (0);
    write(fd, joined, ft_strlen(joined));
    free(joined);
    if (had_nl)
        write(fd, "\n", 1);
    return (1);
}

static int	translate_file(const char *in_path, const char *out_path, t_vec3 delta)
{
    int		fd_in;
    int		fd_out;
    char	*line;
    int		has_nl;
    size_t	len;

    fd_in = open(in_path, O_RDONLY);
    if (fd_in < 0)
        return (perror("move_rt"), 0);
    fd_out = open(out_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out < 0)
        return (perror("move_rt"), close(fd_in), 0);
    line = get_next_line(fd_in);
    while (line)
    {
        len = ft_strlen(line);
        has_nl = 0;
        if (len > 0 && line[len - 1] == '\n')
        {
            has_nl = 1;
            line[len - 1] = '\0';
        }
        if (!process_line(fd_out, line, has_nl, delta))
        {
            free(line);
            close(fd_in);
            close(fd_out);
            return (0);
        }
        free(line);
        line = get_next_line(fd_in);
    }
    close(fd_in);
    close(fd_out);
    return (1);
}

int	main(int argc, char **argv)
{
    t_vec3	delta;

    if (argc != 6)
    {
        print_error("Usage: move_rt <input.rt> <output.rt> dx dy dz");
        return (1);
    }
    if (!parse_float_value(argv[3], &delta.x)
        || !parse_float_value(argv[4], &delta.y)
        || !parse_float_value(argv[5], &delta.z))
    {
        print_error("move_rt: invalid translation values");
        return (1);
    }
    if (!translate_file(argv[1], argv[2], delta))
        return (1);
    return (0);
}
