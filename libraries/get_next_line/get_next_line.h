/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juan <juan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 16:54:14 by jmondon           #+#    #+#             */
/*   Updated: 2025/08/03 23:31:04 by juan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif
# include <stdlib.h>
# include <unistd.h>

typedef struct s_list
{
	char			str[BUFFER_SIZE + 1];
	struct s_list	*next;
}				t_list_gnl;

char		*get_next_line(int fd);

int			ft_read_til_nl(char **nl_pos, t_list_gnl **frst_nd, int fd,
				t_list_gnl **list);

char		*ft_pnt_nl(char *str);

int			ft_fwd_lst(t_list_gnl **list, t_list_gnl **frst_nd);

void		*ft_free_lst(t_list_gnl *lst);

t_list_gnl	*ft_get_tail(t_list_gnl *list, char *nl_pos);

size_t		ft_str_len(const char *str);

char		*ft_get_line(t_list_gnl *frst_nd);

size_t		ft_ln_len(t_list_gnl *lst);

#endif
