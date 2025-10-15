NAME	=	obj_to_rt
CC	=	cc
CFLAGS	=	-Wall -Wextra -Werror -I../../../include -I../../../libraries/libft -I../../../libraries/get_next_line
LDLIBS	=	-lm

SRC	=	obj_to_rt.c
OBJ	=	$(SRC:.c=.o)

LIBFT_DIR	=	../../../libraries/libft
LIBFT	=	$(LIBFT_DIR)/libft.a
GNL_DIR	=	../../../libraries/get_next_line
GNL	=	$(GNL_DIR)/get_next_line.a

all: $(NAME)

$(NAME): $(OBJ) $(LIBFT) $(GNL)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT) $(GNL) $(LDLIBS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(GNL):
	$(MAKE) -C $(GNL_DIR)

clean:
	rm -f $(OBJ)
	$(MAKE) -C $(LIBFT_DIR) clean
	$(MAKE) -C $(GNL_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean
	$(MAKE) -C $(GNL_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
