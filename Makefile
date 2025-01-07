CC = cc
NAME_IDLE = idle_and_audio
NAME_BACKLIGHT = backlight
CFLAGS = -Wall -Wextra -Werror -I. -I/usr/include/libevdev-1.0
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -levdev -lX11 -lXss -lpulse-simple -lpulse
SRC_IDLE =	./utils_idle_and_audio_activity_check.c \
	  		./idle_and_audio_activity_check.c
SRC_BACKLIGHT = ./dynamic_backlight_control.c \
				./utils_backlight_control.c
OBJ_IDLE = $(SRC_IDLE:.c=.o)
OBJ_BACKLIGHT = $(SRC_BACKLIGHT:.c=.o)
all: $(NAME_IDLE) $(NAME_BACKLIGHT)
$(NAME_IDLE): $(OBJ_IDLE)
	@echo "$(BLUE)Compiling idle$(NAME_IDLE)...$(RESET)"
	@$(CC) $(OBJ_IDLE) $(LDFLAGS) -o $(NAME_IDLE)
	@echo "$(GREEN)Compilation of idle successful!$(RESET)"

$(NAME_BACKLIGHT): $(OBJ_BACKLIGHT)
	@echo "$(BLUE)Compiling $(NAME_BACKLIGHT)...$(RESET)"
	@$(CC) $(OBJ_BACKLIGHT) $(LDFLAGS) -o $(NAME_BACKLIGHT)
	@echo "$(GREEN)Compilation of idle successful!$(RESET)"

%.o: %.c
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -f $(OBJ_IDLE) $(OBJ_BACKLIGHT)

fclean: clean
	@echo "$(RED)Cleaning executable $(NAME)...$(RESET)"
	@rm -f $(NAME_IDLE) $(NAME_BACKLIGHT)

re: fclean all

.PHONY: all clean fclean re

RESET  = \033[0m
RED    = \033[31m
GREEN  = \033[32m
YELLOW = \033[33m
BLUE   = \033[34m
BOLD   = \033[1m
