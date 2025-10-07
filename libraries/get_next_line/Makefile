NAME = get_next_line.a
SRC_DIR = .
OBJ_DIR = obj

SRC_FILES =	get_next_line.c get_next_line_utils.c
BONUS_SRC_FILES = get_next_line_bonus.c get_next_line_utils_bonus.c

HEAD = get_next_line.h get_next_line_bonus.h

CC = cc
CFLAGS = -Wall -Wextra -Werror

AR = ar rcs
RM = rm -f

GREEN = \033[0;32m
RED = \033[0;31m
BLUE = \033[0;34m
RESET = \033[0m

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))
BONUS_OBJ = $(addprefix $(OBJ_DIR)/, $(BONUS_SRC_FILES:.c=.o))

all: $(NAME)

bonus: OBJ_BONUS
	@echo "$(BLUE)Building BONUS library $(NAME)...$(RESET)"
	@$(AR) $(NAME) $(BONUS_OBJ)
	@echo "$(GREEN)✓ $(NAME) BONUS successfully built!$(RESET)"

OBJ_BONUS: $(OBJ_DIR) $(BONUS_OBJ)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c $(HEAD) | $(OBJ_DIR)
	@echo "$(GREEN)Compiling $<$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ_DIR) $(OBJ)
	@echo "$(BLUE)Building library $(NAME)...$(RESET)"
	@$(AR) $(NAME) $(OBJ)
	@echo "$(GREEN)✓ $(NAME) successfully built!$(RESET)"

clean:
	@echo "$(RED)Deleting object files...$(RESET)"
	@$(RM) -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Deleting $(NAME)...$(RESET)"
	@$(RM) $(NAME)

re:
	@$(MAKE) fclean
	@$(MAKE) all

info:
	@echo "Source files: $(words $(SRC_FILES))"
	@echo "Bonus source files: $(words $(BONUS_SRC_FILES))"
	@echo "Object folder: $(OBJ_DIR)"

.PHONY: all bonus clean fclean re info
