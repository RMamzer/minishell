/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:59:35 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/12 17:58:47 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <readline/history.h>  // add_history
# include <readline/readline.h> // readline
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# define SUCCESS 0
# define FAILURE 1

# define TO_TRIM " \t\n" // check this

// token type
typedef enum e_token_type
{
	PIPE,
	IN,
	OUT,
	WORD,
	HEREDOC,
	APPEND

}					t_token_type;

typedef struct s_token
{
	t_token_type	type;
	struct s_token	*next;
	char			*content;
}					t_token;

// core
typedef struct s_shell
{
	int				exit_code;
	char			*input_line;
	t_token			*token_list;

}					t_shell;

// main things
int					main(int ac, char **av, char **env);
t_shell				*init_data(void);

// pre processing of input
bool				process_input(char *input_line, t_shell *data);
bool				valid_pipe_usage(char *line);
bool				valid_redirection_usage(char *line);
char				check_quote(char *line, int index);

// lexer
void				lexer(char *input_line, t_shell *data);
size_t				handle_operator(char *input_line, size_t i, t_shell *data);
size_t				handle_word(char *input_line, size_t start, t_shell *data);
void				add_token(t_shell *data, t_token_type type, char *content);

// helper functions
bool				is_delimiter(int i);
bool				is_operator(char c);

// errors
void				show_error(char *msg, int exit_code);
void				lexer_error(char *input_line, t_shell *data);
void				free_list(t_token **list);

#endif
