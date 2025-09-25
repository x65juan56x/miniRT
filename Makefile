NAME            := miniRT
CC              := cc
CFLAGS          := -Wall -Wextra -Werror -MMD -MP
INCLUDES        := -I include -I libraries/MLX42/include -I libraries/MLX42/include/MLX42
LDFLAGS         := -ldl -lglfw -pthread -lm -lGL

SRC_DIR         := src
OBJ_DIR         := obj

SRCS            := $(shell find $(SRC_DIR) -name '*.c')
OBJS            := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS            := $(OBJS:.o=.d)

MLX_DIR         := libraries/MLX42
MLX_BUILD_DIR   := $(MLX_DIR)/build
MLX_LIB         := $(MLX_BUILD_DIR)/libmlx42.a

all: $(NAME)

$(NAME): $(MLX_LIB) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(MLX_LIB) $(LDFLAGS) -o $@

$(MLX_LIB):
	cmake -S $(MLX_DIR) -B $(MLX_BUILD_DIR) -DMLX_BUILD_EXAMPLES=OFF
	cmake --build $(MLX_BUILD_DIR) --parallel

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

bonus: all

-include $(DEPS)

.PHONY: all clean fclean re bonus
