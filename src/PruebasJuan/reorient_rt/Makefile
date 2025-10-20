NAME := reorient_rt
CC := cc
CFLAGS := -Wall -Wextra -Werror -std=c99
LDFLAGS := -lm

SRCS := reorient_rt.c
OBJS := $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

