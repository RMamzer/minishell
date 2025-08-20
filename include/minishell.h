/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:59:35 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/20 16:10:25 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <errno.h>
# include <readline/history.h>  // add_history
# include <readline/readline.h> // readline
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# define SUCCESS 0
# define FAILURE 1
# define IN_DOUBLE_QUOTE true
# define NO_QUOTE false

# define ALLOC true
# define NO_ALLOC false

# define TO_TRIM " \t\n" // check this

// token type
typedef enum e_token_type
{
	PIPE,
	IN,
	OUT,
	WORD,
	HEREDOC,
	APPEND,
	HEREDOC_DELIM_QT,
	HEREDOC_DELIM_UQ
}					t_token_type;

typedef struct s_token
{
	t_token_type	type;
	struct s_token	*next;
	char			*content;
}					t_token;

// env struct
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}					t_env;

// core
typedef struct s_shell
{
	int				exit_code;
	char			*input_line;
	t_token			*token_list;
	t_env			*env;

}					t_shell;

// main things
int					main(int ac, char **av, char **env);
t_shell				*init_data(void);

// pre processing of input
bool				process_input(char *input_line, t_shell *data);
char				check_quote(char *line, int index);

// lexer
void				lexer(char *input_line, t_shell *data);
size_t				handle_operator(char *input_line, size_t i, t_shell *data);
size_t				handle_word(char *input_line, size_t start, t_shell *data);
void				add_token(t_shell *data, t_token_type type, char *content);
void				check_heredoc(t_shell *data);
bool				check_syntax(t_shell *data);

// expansion
void				expander(t_shell *data);
char				*expand_content(char *content, t_shell *data);
char				*process_content(char *content, size_t *i, t_shell *data);
char				*handle_dollar(char *content, size_t *i, t_shell *data);
char				*handle_characters(char *content, size_t *i, bool in_dq);
char				*expand_env_var(char *content, size_t *i, t_env *env);
char				*get_env_value(char *name, t_env *env, bool alloc);
char				*strjoin_free(char *new_content, char *temp);
int					ft_strcmp(const char *s1, const char *s2);

char				*handle_single_quote(char *content, size_t *i);
char				*handle_double_quote(char *content, size_t *i,
						t_shell *data);

// helper functions
bool				is_delimiter(int i);
bool				is_operator(char c);

// errors
void				show_error(char *msg, int exit_code);
void				lexer_error(char *input_line, t_shell *data);
void				free_list(t_token **list);

// env
void				error_exit(char *msg);
void				error_env_exit(char *key, char *value, t_shell *shell);
bool				update_env_value(t_env **env, char *key, char *new_value);
char				*find_env_value(char *str, t_env *env);
void				update_shllvl_value(t_shell *shell);
void				add_env_node(t_env **env, t_env *new_node);
t_env				*create_env_node(char *key, char *value);
void				process_env_line(t_shell *shell, const char *envp);
void				set_minimal_env(t_shell *shell);
void				create_env(t_shell *shell, char **envp);

#endif
