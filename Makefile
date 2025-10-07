NAME = miniRT
CC = cc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I include -I libraries/MLX42/include -I libraries/MLX42/include/MLX42 -I libraries/libft
LDFLAGS = -ldl -lglfw -pthread -lm -lGL

SRC_DIR = src
OBJ_DIR = obj

# All C sources under src, excluding the parser test main from the primary binary
SRCS_ALL = $(shell find $(SRC_DIR) -name '*.c')
SRCS = $(filter-out $(SRC_DIR)/parser_main.c,$(SRCS_ALL))
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# libft
LIBFT_DIR = libraries/libft
LIBFT_LIB = $(LIBFT_DIR)/libft.a

# get_next_line
GNL_DIR = libraries/get_next_line
GNL_LIB = $(GNL_DIR)/get_next_line.a

MLX_DIR = libraries/MLX42
MLX_BUILD_DIR = $(MLX_DIR)/build
MLX_LIB = $(MLX_BUILD_DIR)/libmlx42.a

all: $(NAME)

$(NAME): $(MLX_LIB) $(LIBFT_LIB) $(GNL_LIB) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(MLX_LIB) $(LIBFT_LIB) $(GNL_LIB) $(LDFLAGS) -o $@

$(MLX_LIB):
	cmake -S $(MLX_DIR) -B $(MLX_BUILD_DIR) -DMLX_BUILD_EXAMPLES=OFF
	cmake --build $(MLX_BUILD_DIR) --parallel

$(LIBFT_LIB):
	$(MAKE) -C $(LIBFT_DIR)

$(GNL_LIB):
	$(MAKE) -C $(GNL_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean
	$(MAKE) -C $(GNL_DIR) clean

fclean: clean
	rm -f $(NAME) parser
	$(MAKE) -C $(LIBFT_DIR) fclean
	$(MAKE) -C $(GNL_DIR) fclean

re:
	$(MAKE) fclean
	$(MAKE) all

bonus: all

-include $(DEPS)

.PHONY: all clean fclean re bonus

# -----------------
# Parser test target
# -----------------
# Build a lightweight parser CLI without MLX42
PARSER_SRCS = $(wildcard $(SRC_DIR)/parse/*.c) \
		   $(SRC_DIR)/parser_main.c \
		   $(SRC_DIR)/math/vec3.c \
		   $(SRC_DIR)/core/scene.c
PARSER_OBJS = $(PARSER_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

parser: $(LIBFT_LIB) $(GNL_LIB) $(PARSER_OBJS)
	$(CC) $(CFLAGS) $(PARSER_OBJS) $(LIBFT_LIB) $(GNL_LIB) -lm -o $@
