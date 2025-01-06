CC = cc
NAME = power_saver
CFLAGS = -Wall -Wextra -Werror -I. -I/usr/include/libevdev-1.0
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -levdev
SRC = ./main.c \
      ./utils_main.c \
	  ./dynamic_backlight_control.c \
	  ./utils_backlight_control.c \
	  ./utils_idle_and_audio_activity_check.c \
	  ./idle_and_audio_activity_check.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(BLUE)Compiling $(NAME)...$(RESET)"
	@$(CC) $(OBJ) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)Compilation successful!$(RESET)"

%.o: %.c
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -f $(OBJ)

fclean: clean
	@echo "$(RED)Cleaning executable $(NAME)...$(RESET)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

RESET  = \033[0m
RED    = \033[31m
GREEN  = \033[32m
YELLOW = \033[33m
BLUE   = \033[34m
BOLD   = \033[1m
