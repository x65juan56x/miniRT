NAME		= move_rt
SRCS		= move_rt.c
OBJDIR		= obj
OBJS		= $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))

CC			= cc
CFLAGS		= -Wall -Wextra -Werror
INCLUDES	= -I../../../include -I../../../libraries/libft -I../../../libraries/get_next_line
LIBS		= ../../../libraries/libft/libft.a \
			  ../../../libraries/get_next_line/get_next_line.a

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(NAME)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: $(NAME)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
