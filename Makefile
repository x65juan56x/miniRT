NAME = miniRT
CC = cc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I include -I libraries/MLX42/include -I libraries/MLX42/include/MLX42 -I libraries/libft
LDFLAGS = -ldl -lglfw -pthread -lm -lGL

SRC_DIR = src
OBJ_DIR = obj

# Parsing related sources
PARSE_SRCS = \
	$(SRC_DIR)/parse/free_lines.c \
	$(SRC_DIR)/parse/parse_dispatch.c \
	$(SRC_DIR)/parse/parse_elements.c \
	$(SRC_DIR)/parse/parse_numbers.c \
	$(SRC_DIR)/parse/parse_objects.c \
	$(SRC_DIR)/parse/parse_result.c \
	$(SRC_DIR)/parse/parser.c \
	$(SRC_DIR)/parse/parser_utils.c \
	$(SRC_DIR)/parse/parse_vectors.c \
	$(SRC_DIR)/parse/token_split.c

# Core & math utilities (no main)
CORE_SRCS = \
	$(SRC_DIR)/color/color.c \
	$(SRC_DIR)/core/ray.c \
	$(SRC_DIR)/core/scene.c \
	$(SRC_DIR)/math/vec3.c \
	$(SRC_DIR)/math/trig_aux.c \
	$(SRC_DIR)/math/aux_maths.c \
	$(SRC_DIR)/PruebasJuan/camera_test.c # PRUEBAS JUAN #

# Select the active main (only one file containing main())
MAIN_SRC = $(SRC_DIR)/main_example01.c
# PRUEBAS JUAN #
# MAIN_SRC = $(SRC_DIR)/PruebasJuan/main00_redsphere.c # Esfera roja
# MAIN_SRC = $(SRC_DIR)/PruebasJuan/main01_normalsphere.c # Esfera con color según normales
# MAIN_SRC = $(SRC_DIR)/PruebasJuan/main02_red_t_sphere.c # Esfera roja con profundidad
# MAIN_SRC = $(SRC_DIR)/PruebasJuan/main03_camera_playground.c # Prueba de cámara
# MAIN_SRC = $(SRC_DIR)/PruebasJuan/main04_parser_playground.c # Prueba del parser (pasar archivo.rt desde la terminal) ejemplo: ./miniRT src/PruebasJuan/ejemplos/parser_playground.rt

# Final sources for the primary miniRT binary
SRCS = $(PARSE_SRCS) $(CORE_SRCS) $(MAIN_SRC)

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
# Build a lightweight parser CLI without MLX42 (uses parser_main)
PARSER_MAIN = $(SRC_DIR)/parser_main.c
PARSER_SRCS = $(PARSE_SRCS) $(PARSER_MAIN) $(SRC_DIR)/math/vec3.c $(SRC_DIR)/core/scene.c
PARSER_OBJS = $(PARSER_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

parser: $(LIBFT_LIB) $(GNL_LIB) $(PARSER_OBJS)
	$(CC) $(CFLAGS) $(PARSER_OBJS) $(LIBFT_LIB) $(GNL_LIB) -lm -o $@
