NAME = minishell

CC = cc
# remove -g
C_FLAGS = -Wall -Wextra -Werror -g

LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a


OBJS_DIR = objs/
SRCS_DIR = srcs/

RESET =	\033[0m
RED = \033[31m

HEADERS = -I ./include  -I $(LIBFT_DIR)

SRCS = builtin_cd.c builtin_echo.c builtin_env.c builtin_exit.c builtin_export.c builtin_export_utils.c \
builtin_pwd.c external_cmd.c builtin_unset.c env_creation.c env_utils.c execute.c execute_utils.c pipe.c \
redirection.c expander.c ft_split_IFS.c lexer.c parser.c split_variables.c main.c checkers.c exit.c \
heredoc.c is_helper.c confirm_syntax.c parser_node_creation.c expander_handlers.c delete_empty_tokens.c \
heredoc_read_loop.c heredoc_expander.c heredoc_save_file.c free.c free_structs.c 


OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.c=.o))
.SECONDARY: $(OBJS)

all: $(NAME)
	@printf "$(RED)▓█████▄  ██▓ ██▀███  ▄▄▄█████▓▓██   ██▓        ██████  ██░ ██ ▓█████  ██▓     ██▓           ██▀███  ▓█████ ▄▄▄      ▓█████▄▓██   ██▓   \n$(RESET)"
	@printf "$(RED)▒██▀ ██▌▓██▒▓██ ▒ ██▒▓  ██▒ ▓▒ ▒██  ██▒      ▒██    ▒ ▓██░ ██▒▓█   ▀ ▓██▒    ▓██▒          ▓██ ▒ ██▒▓█   ▀▒████▄    ▒██▀ ██▌▒██  ██▒   \n$(RESET)"
	@printf "$(RED)░██   █▌▒██▒▓██ ░▄█ ▒▒ ▓██░ ▒░  ▒██ ██░      ░ ▓██▄   ▒██▀▀██░▒███   ▒██░    ▒██░          ▓██ ░▄█ ▒▒███  ▒██  ▀█▄  ░██   █▌ ▒██ ██░   \n$(RESET)"
	@printf "$(RED)░▓█▄   ▌░██░▒██▀▀█▄  ░ ▓██▓ ░   ░ ▐██▓░        ▒   ██▒░▓█ ░██ ▒▓█  ▄ ▒██░    ▒██░          ▒██▀▀█▄  ▒▓█  ▄░██▄▄▄▄██ ░▓█▄   ▌ ░ ▐██▓░   \n$(RESET)"
	@printf "$(RED)░▒████▓ ░██░░██▓ ▒██▒  ▒██▒ ░   ░ ██▒▓░      ▒██████▒▒░▓█▒░██▓░▒████▒░██████▒░██████▒      ░██▓ ▒██▒░▒████▒▓█   ▓██▒░▒████▓  ░ ██▒▓░   \n$(RESET)"
	@printf "$(RED) ▒▒▓  ▒ ░▓  ░ ▒▓ ░▒▓░  ▒ ░░      ██▒▒▒       ▒ ▒▓▒ ▒ ░ ▒ ░░▒░▒░░ ▒░ ░░ ▒░▓  ░░ ▒░▓  ░      ░ ▒▓ ░▒▓░░░ ▒░ ░▒▒   ▓▒█░ ▒▒▓  ▒   ██▒▒▒    \n$(RESET)"
	@printf "$(RED) ░ ▒  ▒  ▒ ░  ░▒ ░ ▒░    ░     ▓██ ░▒░       ░ ░▒  ░ ░ ▒ ░▒░ ░ ░ ░  ░░ ░ ▒  ░░ ░ ▒  ░        ░▒ ░ ▒░ ░ ░  ░ ▒   ▒▒ ░ ░ ▒  ▒ ▓██ ░▒░    \n$(RESET)"
	@printf "$(RED) ░ ░  ░  ▒ ░  ░░   ░   ░       ▒ ▒ ░░        ░  ░  ░   ░  ░░ ░   ░     ░ ░     ░ ░           ░░   ░    ░    ░   ▒    ░ ░  ░ ▒ ▒ ░░     \n$(RESET)"
	@printf "$(RED)   ░     ░     ░               ░ ░                 ░   ░  ░  ░   ░  ░    ░  ░    ░  ░         ░        ░  ░     ░  ░   ░    ░ ░        \n$(RESET)"
	@printf "$(RED) ░                             ░ ░                                                                                   ░      ░ ░     \n$(RESET)"
$(LIBFT):
	make -C $(LIBFT_DIR)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c
	mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) -c $< -o $@ $(HEADERS)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(C_FLAGS) $(OBJS) $(LIBFT)  -lreadline -o $(NAME)

clean:
	rm -rf $(OBJS_DIR)
	make clean -C $(LIBFT_DIR)

fclean: clean
	rm -f $(NAME) $(LIBFT)

re: fclean all

.PHONY: all re clean fclean