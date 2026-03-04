NAME :=	Famine

CC :=	cc
CFLAGS :=	-g -MP -MMD -Wall -Wextra -Werror # -fsanitize=address -fno-omit-frame-pointer
LFLAGS :=

###

INCLUDE_DIRS :=	inc/\

SRCS :=	main\
		strjoin\
		infect\
		check\

###

INCLUDE_DIRS :=	$(addprefix -I, $(INCLUDE_DIRS))

SRCS :=	$(addprefix src/, $(SRCS))
SRCS :=	$(addsuffix .c, $(SRCS))

###

OBJ_DIR :=	obj

OBJS =	$(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS =	$(SRCS:%.c=$(OBJ_DIR)/%.d)

###

compile:
	@make -j all --no-print-directory

all: $(NAME)

$(NAME): $(OBJS)
	@echo Compiling $(NAME)
	@$(CC) $(CFLAGS) $(LFLAGS) $(INCLUDE_DIRS) -o $@ $^

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo Compiling $@
	@$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

re: fclean compile

fclean: clean
	@echo Removed $(NAME)
	@rm -rf $(NAME)

clean:
	@echo Removed $(OBJ_DIR)
	@rm -rf $(OBJ_DIR)

.PHONY: all clean fclean re compile

-include $(DEPS)
