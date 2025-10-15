/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmondon <jmondon@student.42malaga.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/07 16:54:14 by jmondon           #+#    #+#             */
/*   Updated: 2025/05/16 20:52:02 by jmondon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif
# include <stdlib.h>
# include <unistd.h>

typedef struct s_list
{
	char			str[BUFFER_SIZE + 1];
	struct s_list	*next;
}				t_list;

char	*get_next_line(int fd);

int		ft_read_til_nl(char **nl_pos, t_list **frst_nd, int fd, t_list **list);

char	*ft_pnt_nl(char *str);

int		ft_fwd_lst(t_list **list, t_list **frst_nd);

void	*ft_free_lst(t_list *lst);

t_list	*ft_get_tail(t_list *list, char *nl_pos);

size_t	ft_str_len(const char *str);

char	*ft_get_line(t_list *frst_nd);

size_t	ft_ln_len(t_list *lst);

#endif
