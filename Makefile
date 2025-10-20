NAME        = miniRT
NAME_BONUS  = miniRT_bonus
CC          = cc
CFLAGS      = -Wall -Wextra -Werror
CFLAGS_BONUS = $(CFLAGS) -DSCENE_HEADER='"scene_bonus.h"'
INCLUDES    = -I include -I libraries/MLX42/include -I libraries/MLX42/include/MLX42 -I libraries/libft
LDFLAGS     = -ldl -lglfw -pthread -lm -lGL -Ofast -march=native -O3 -ffast-math

SRC_DIR     = src
OBJ_DIR     = obj

# Common (shared by both builds)
COMMON_SRCS = \
	$(SRC_DIR)/color/color.c \
	$(SRC_DIR)/core/ray.c \
	$(SRC_DIR)/core/scene.c \
	$(SRC_DIR)/math/vec3.c \
	$(SRC_DIR)/math/math_utils.c \
	$(SRC_DIR)/camera/camera.c \
	$(SRC_DIR)/render/framebuffer.c \
	$(SRC_DIR)/render/render.c \
	$(SRC_DIR)/shading/lambert.c \
	$(SRC_DIR)/shading/shadow.c \
	$(SRC_DIR)/app/input.c \
	$(SRC_DIR)/app/toggle_info.c

# ---------- Mandatory set ----------
PARSE_M_SRCS = \
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

GEOM_M_SRCS = \
	$(SRC_DIR)/geom/sphere.c \
	$(SRC_DIR)/geom/plane.c \
	$(SRC_DIR)/geom/cylinder.c

CORE_M_SRCS = \
	$(SRC_DIR)/core/intersect.c

MAIN_M      = $(SRC_DIR)/minirt.c
SRCS_M      = $(PARSE_M_SRCS) $(COMMON_SRCS) $(GEOM_M_SRCS) $(CORE_M_SRCS) $(MAIN_M)
OBJS_M      = $(SRCS_M:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# ---------- Bonus set ----------
PARSE_B_SRCS = \
	$(SRC_DIR)/parse/free_lines.c \
	$(SRC_DIR)/parse/parse_dispatch_bonus.c \
	$(SRC_DIR)/parse/parse_elements.c \
	$(SRC_DIR)/parse/parse_numbers.c \
	$(SRC_DIR)/parse/parse_objects_bonus.c \
	$(SRC_DIR)/parse/parse_result.c \
	$(SRC_DIR)/parse/parser.c \
	$(SRC_DIR)/parse/parser_utils.c \
	$(SRC_DIR)/parse/parse_vectors.c \
	$(SRC_DIR)/parse/token_split.c

GEOM_B_SRCS = \
	$(SRC_DIR)/geom/sphere.c \
	$(SRC_DIR)/geom/plane.c \
	$(SRC_DIR)/geom/cylinder.c \
	$(SRC_DIR)/geom/triangle_bonus.c \
	$(SRC_DIR)/geom/hparaboloid_bonus.c

CORE_B_SRCS = \
	$(SRC_DIR)/core/intersect_bonus.c

MAIN_B      = $(SRC_DIR)/minirt_bonus.c
SRCS_B      = $(PARSE_B_SRCS) $(COMMON_SRCS) $(GEOM_B_SRCS) $(CORE_B_SRCS) $(MAIN_B)
OBJS_B      = $(SRCS_B:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.bo)

DEPS_M      = $(OBJS_M:.o=.d)
DEPS_B      = $(OBJS_B:.bo=.d)

# Libraries
LIBFT_DIR   = libraries/libft
LIBFT_LIB   = $(LIBFT_DIR)/libft.a
GNL_DIR     = libraries/get_next_line
GNL_LIB     = $(GNL_DIR)/get_next_line.a
MLX_DIR     = libraries/MLX42
MLX_BUILD_DIR = $(MLX_DIR)/build
MLX_LIB     = $(MLX_BUILD_DIR)/libmlx42.a

all: $(NAME)

$(NAME): $(MLX_LIB) $(LIBFT_LIB) $(GNL_LIB) $(OBJS_M)
	$(CC) $(CFLAGS) $(OBJS_M) $(MLX_LIB) $(LIBFT_LIB) $(GNL_LIB) $(LDFLAGS) -o $@

bonus: $(NAME_BONUS)

$(NAME_BONUS): $(MLX_LIB) $(LIBFT_LIB) $(GNL_LIB) $(OBJS_B)
	$(CC) $(CFLAGS_BONUS) $(filter %.bo,$(OBJS_B)) $(MLX_LIB) $(LIBFT_LIB) $(GNL_LIB) $(LDFLAGS) -o $@

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
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

$(OBJ_DIR)/%.bo: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS_BONUS) $(INCLUDES) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean
	$(MAKE) -C $(GNL_DIR) clean

fclean: clean
	rm -f $(NAME) $(NAME_BONUS) parser
	$(MAKE) -C $(LIBFT_DIR) fclean
	$(MAKE) -C $(GNL_DIR) fclean

re:
	$(MAKE) fclean
	$(MAKE) all

-include $(DEPS_M)
-include $(DEPS_B)

.PHONY: all clean fclean re bonus
