NAME            = miniRT
NAME_BONUS      = miniRT_bonus
CC              = cc
CFLAGS          = -Wall -Wextra -Werror -Ofast -O3 -ffast-math -march=native
INCLUDES        = -I include -I libraries/MLX42/include -I libraries/MLX42/include/MLX42 -I libraries/libft
INCLUDES_BONUS  = $(INCLUDES) -I include_bonus
LDFLAGS         = -ldl -lglfw -pthread -lm -lGL

SRC_DIR_M       = src
SRC_DIR_B       = src_bonus
OBJ_ROOT        = obj
OBJ_DIR_M       = $(OBJ_ROOT)/mandatory
OBJ_DIR_B       = $(OBJ_ROOT)/bonus

# Common core (shared by both builds)
COMMON_CORE_SRCS = \
	$(SRC_DIR_M)/color/color.c \
	$(SRC_DIR_M)/core/ray.c \
	$(SRC_DIR_M)/math/vec3.c \
	$(SRC_DIR_M)/math/vec3_arith.c \
	$(SRC_DIR_M)/math/vec3_const.c \
	$(SRC_DIR_M)/math/math_utils.c

# ---------- Mandatory set ----------
PARSE_M_SRCS = \
	$(SRC_DIR_M)/parse/free_lines.c \
	$(SRC_DIR_M)/parse/parse_dispatch.c \
	$(SRC_DIR_M)/parse/parse_elements.c \
	$(SRC_DIR_M)/parse/parse_numbers.c \
	$(SRC_DIR_M)/parse/parse_objects.c \
	$(SRC_DIR_M)/parse/parse_result.c \
	$(SRC_DIR_M)/parse/parser.c \
	$(SRC_DIR_M)/parse/parser_utils.c \
	$(SRC_DIR_M)/parse/parse_vectors.c \
	$(SRC_DIR_M)/parse/token_split.c \
	$(SRC_DIR_M)/parse/calc_aux_vars.c

GEOM_M_SRCS = \
	$(SRC_DIR_M)/geom/sphere.c \
	$(SRC_DIR_M)/geom/plane.c \
	$(SRC_DIR_M)/geom/cylinder.c \
	$(SRC_DIR_M)/geom/cylinder_utils.c

CORE_M_SRCS = \
	$(SRC_DIR_M)/core/intersect.c \
	$(SRC_DIR_M)/core/hit.c \
	$(SRC_DIR_M)/core/scene.c \
	$(SRC_DIR_M)/camera/camera.c \
	$(SRC_DIR_M)/app/input.c \
	$(SRC_DIR_M)/app/app_utils.c \
	$(SRC_DIR_M)/render/framebuffer.c \
	$(SRC_DIR_M)/render/render.c

MAIN_M      = $(SRC_DIR_M)/minirt.c
SRCS_M      = $(PARSE_M_SRCS) $(COMMON_CORE_SRCS) $(SRC_DIR_M)/shading/lambert.c $(SRC_DIR_M)/shading/shadow.c $(GEOM_M_SRCS) $(CORE_M_SRCS) $(MAIN_M)
OBJS_M      = $(SRCS_M:$(SRC_DIR_M)/%.c=$(OBJ_DIR_M)/%.o)

# ---------- Bonus set ----------
PARSE_B_SRCS = \
	$(SRC_DIR_B)/parse/free_lines_bonus.c \
	$(SRC_DIR_B)/parse/parser_bonus.c \
	$(SRC_DIR_B)/parse/parse_dispatch_bonus.c \
	$(SRC_DIR_B)/parse/parse_elements_bonus.c \
	$(SRC_DIR_B)/parse/parse_element_lights_bonus.c \
	$(SRC_DIR_B)/parse/parse_numbers_bonus.c \
	$(SRC_DIR_B)/parse/parse_objects_bonus.c \
	$(SRC_DIR_B)/parse/parse_sphere_bonus.c \
	$(SRC_DIR_B)/parse/parse_plane_bonus.c \
	$(SRC_DIR_B)/parse/parse_cylinder_bonus.c \
	$(SRC_DIR_B)/parse/parse_hparaboloid_bonus.c \
	$(SRC_DIR_B)/parse/parse_triangle_bonus.c \
	$(SRC_DIR_B)/parse/parse_specular_bonus.c \
	$(SRC_DIR_B)/parse/parse_specular_utils_bonus.c \
	$(SRC_DIR_B)/parse/parse_vectors_bonus.c \
	$(SRC_DIR_B)/parse/parser_utils_bonus.c \
	$(SRC_DIR_B)/parse/token_split_bonus.c \
	$(SRC_DIR_B)/parse/parse_result_bonus.c \
	$(SRC_DIR_B)/parse/calc_aux_vars_bonus.c

GEOM_B_SRCS = \
	$(SRC_DIR_B)/geom/sphere_bonus.c \
	$(SRC_DIR_B)/geom/plane_bonus.c \
	$(SRC_DIR_B)/geom/cylinder_bonus.c \
	$(SRC_DIR_B)/geom/triangle_bonus.c \
	$(SRC_DIR_B)/geom/hparaboloid_bonus.c \
	$(SRC_DIR_B)/geom/cylinder_utils_bonus.c \
	$(SRC_DIR_B)/geom/hparaboloid_utils_bonus.c

CORE_B_SRCS = \
	$(SRC_DIR_B)/core/scene_bonus.c \
	$(SRC_DIR_B)/core/intersect_bonus.c \
	$(SRC_DIR_B)/core/free_objects_bonus.c \
	$(SRC_DIR_B)/core/intersect_common_bonus.c \
	$(SRC_DIR_B)/core/intersect_sphere_bonus.c \
	$(SRC_DIR_B)/core/intersect_plane_bonus.c \
	$(SRC_DIR_B)/core/intersect_triangle_bonus.c \
	$(SRC_DIR_B)/core/intersect_hparaboloid_bonus.c \
	$(SRC_DIR_B)/core/intersect_cylinder_bonus.c \
	$(SRC_DIR_B)/core/intersect_cylinder_wall_bonus.c \
	$(SRC_DIR_B)/core/intersect_cylinder_caps_bonus.c \
	$(SRC_DIR_B)/app/input_bonus.c \
	$(SRC_DIR_B)/app/app_utils_bonus.c \
	$(SRC_DIR_B)/camera/camera_bonus.c \
	$(SRC_DIR_B)/render/framebuffer_bonus.c \
	$(SRC_DIR_B)/render/render_bonus.c \
	$(SRC_DIR_B)/shading/lambert_bonus.c \
	$(SRC_DIR_B)/shading/shadow_bonus.c \
	$(SRC_DIR_B)/shading/bump_bonus.c \
	$(SRC_DIR_B)/shading/specular_bonus.c

MAIN_B      = $(SRC_DIR_B)/minirt_bonus.c
SRCS_B      = $(PARSE_B_SRCS) $(COMMON_CORE_SRCS) $(GEOM_B_SRCS) $(CORE_B_SRCS) $(MAIN_B)
PARSE_B_OBJS = $(PARSE_B_SRCS:$(SRC_DIR_B)/%.c=$(OBJ_DIR_B)/%.o)
GEOM_B_OBJS  = $(GEOM_B_SRCS:$(SRC_DIR_B)/%.c=$(OBJ_DIR_B)/%.o)
CORE_B_OBJS  = $(CORE_B_SRCS:$(SRC_DIR_B)/%.c=$(OBJ_DIR_B)/%.o)
MAIN_B_OBJ   = $(MAIN_B:$(SRC_DIR_B)/%.c=$(OBJ_DIR_B)/%.o)
COMMON_B_OBJS = $(COMMON_CORE_SRCS:$(SRC_DIR_M)/%.c=$(OBJ_DIR_B)/%.o)
OBJS_B      = $(PARSE_B_OBJS) $(GEOM_B_OBJS) $(CORE_B_OBJS) $(COMMON_B_OBJS) $(MAIN_B_OBJ)

DEPS_M      = $(OBJS_M:.o=.d)
DEPS_B      = $(OBJS_B:.o=.d)

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
	$(CC) $(CFLAGS) $(OBJS_B) $(MLX_LIB) $(LIBFT_LIB) $(GNL_LIB) $(LDFLAGS) -o $@

$(MLX_LIB):
	cmake -S $(MLX_DIR) -B $(MLX_BUILD_DIR) -DMLX_BUILD_EXAMPLES=OFF
	cmake --build $(MLX_BUILD_DIR) --parallel

$(LIBFT_LIB):
	$(MAKE) -C $(LIBFT_DIR)

$(GNL_LIB):
	$(MAKE) -C $(GNL_DIR)

$(OBJ_DIR_M):
	@mkdir -p $(OBJ_DIR_M)

$(OBJ_DIR_B):
	@mkdir -p $(OBJ_DIR_B)

$(OBJ_DIR_M)/%.o: $(SRC_DIR_M)/%.c | $(OBJ_DIR_M)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

$(OBJ_DIR_B)/%.o: $(SRC_DIR_B)/%.c | $(OBJ_DIR_B)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES_BONUS) -MMD -MP -c $< -o $@

$(OBJ_DIR_B)/%.o: $(SRC_DIR_M)/%.c | $(OBJ_DIR_B)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES_BONUS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJ_ROOT)
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
