NAME :=	Famine

CC :=	cc
CFLAGS :=	-MP -MMD -Wall -Wextra -Werror # -fsanitize=address -fno-omit-frame-pointer
LFLAGS :=

###

INCLUDE_DIRS :=	inc/\

SRCS :=	main\
		infect\
		check\
		crawl\
		payload\
		utils\
		daemon\
		service\

###

INCLUDE_DIRS :=	$(addprefix -I, $(INCLUDE_DIRS))

SRCS :=	$(addprefix src/, $(SRCS))
SRCS :=	$(addsuffix .c, $(SRCS))

###

OBJ_DIR :=	obj

OBJS =	$(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS =	$(SRCS:%.c=$(OBJ_DIR)/%.d)

###

TPUT 					= tput -T xterm-256color
_RESET 					:= $(shell $(TPUT) sgr0)
_BOLD 					:= $(shell $(TPUT) bold)
_ITALIC 				:= $(shell $(TPUT) sitm)
_UNDER 					:= $(shell $(TPUT) smul)
_GREEN 					:= $(shell $(TPUT) setaf 2)
_YELLOW 				:= $(shell $(TPUT) setaf 3)
_RED 					:= $(shell $(TPUT) setaf 1)
_GRAY 					:= $(shell $(TPUT) setaf 8)
_PURPLE 				:= $(shell $(TPUT) setaf 5)

compile:
	@make -j all --no-print-directory

all: $(NAME)

$(NAME): $(OBJS)
	@echo 'Linking $(_BOLD)$(NAME)$(_RESET)'
	@/bin/time --format='$(_GREEN)(%es)$(_RESET) Linked $(_BOLD)$(NAME)$(_RESET)' $(CC) $(CFLAGS) $(LFLAGS) $(INCLUDE_DIRS) -o $@ $^

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo 'Compiling $(_BOLD)$<$(_RESET)'
	@/bin/time --format='$(_GREEN)(%es)$(_RESET) Compiled $(_BOLD)$@$(_RESET)' $(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

re: fclean compile

fclean: clean
	@echo 'Removed $(_BOLD)$(NAME)$(_RESET)'
	@rm -rf $(NAME)

clean:
	@echo 'Removed $(_BOLD)$(OBJ_DIR)$(_RESET)'
	@rm -rf $(OBJ_DIR)

.PHONY: all clean fclean re compile

-include $(DEPS)
