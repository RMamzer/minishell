/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:59:35 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/09 12:10:47 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <errno.h>
# include <fcntl.h>             // O_RDONLY abd others
# include <limits.h>            // for exit limits
# include <readline/history.h>  // add_history
# include <readline/readline.h> // readline
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h> //open and close

# define SUCCESS 0
# define FAILURE 1
# define IN_DOUBLE_QUOTE true
# define NO_QUOTE false

# define ALLOC true
# define NO_ALLOC false

# define TO_TRIM " \t\n" // check this

# define ERROR_QUOTE "syntax error: unclosed quote"
# define ERROR_MEM "cannot allocate memory"
# define ERROR_PIPE "syntax error near unexpected token `|\'"
# define ERROR_REDIR "syntax error near unexpected token `redirection'"
# define ERROR_MAX_HER "maximum here-document count exceeded"
// token type
typedef enum e_token_type
{
	PIPE,
	WORD,
	IN,
	OUT,
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
	bool			quoted;
	bool			expanded;
}					t_token;

// env struct
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
	bool			assigned;
}					t_env;

// args instead of
typedef struct s_ast
{
	t_token_type	type;
	char			**value;
	int				expand;
	int				status;
	int				fd[2];
	struct s_ast	*left;
	struct s_ast	*right;
}					t_ast;

// core
typedef struct s_shell
{
	int				exit_code;
	char			*input_line;
	t_token			*token_list;
	t_env			*env;
	t_ast			*ast;
	char			**env_array;
	char			**paths_array;
}					t_shell;

// lib and macro for execution
# include <sys/wait.h>
# define READ_END 0
# define WRITE_END 1

# define SUCCESS 0
# define FAILURE 1

# define ALLOC true
# define NO_ALLOC false

# define EXIT_INVALID_OPTION 2

// main things
int					main(int ac, char **av, char **env);
t_shell				*init_data(void);

// pre processing of input
bool				process_input(char *input_line, t_shell *shell);
char				check_quote(char *line, int index);

// lexer
void				lexer(char *input_line, t_shell *shell);
size_t				handle_operator(char *input_line, size_t i, t_shell *shell);
size_t				handle_word(char *input_line, size_t start, t_shell *shell);
void				add_token(t_shell *shell, t_token_type type, char *content);
void				check_heredoc(t_shell *shell);
bool				check_syntax(t_shell *shell);
void				quote_flag(t_shell *shell);

// expansion
void				expander(t_shell *shell);
char				*expand_content(char *content, t_shell *shell,
						t_token *token);
char				*process_content(char *content, size_t *i, t_shell *shell,
						t_token *token);
char				*handle_dollar(char *content, size_t *i, t_shell *shell,
						t_token *token);
char				*handle_characters(char *content, size_t *i, bool in_dq);
char				*expand_env_var(char *content, size_t *i, t_env *env);
char				*get_env_value(char *name, t_env *env, bool alloc);
char				*strjoin_free(char *new_content, char *temp);
int					ft_strcmp(const char *s1, const char *s2);

char				*handle_single_quote(char *content, size_t *i);
char				*handle_double_quote(char *content, size_t *i,
						t_shell *shell, t_token *token);

// helper functions
bool				is_delimiter(int i);
bool				is_operator(char c);

// errors
void				show_error(char *msg, t_shell *shell, int exit_code);
void				lexer_error(char *input_line, t_shell *shell);
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

// ast
/*
bool				parse_tokens(t_shell *shell);
t_ast				*parse_pipe(t_token **token_list, t_shell *shell);
t_ast				*parse_redirection(t_token **token_list, t_shell *shell);
t_ast	*parse_command_with_redirections(t_token **token_list,
						t_shell *shell);
t_ast	*parse_single_redirection(t_token **token_list,
						t_shell *shell);
int					count_args(t_token *tokens);
void	load_args(t_ast *command_node, t_token **token_list, int ac,
						t_shell *shell);
t_ast				*parse_command(t_token **token_list, t_shell *shell);
t_ast				*add_file_node(t_token *token, t_shell *shell);
t_ast				*add_ast_node(t_token_type type, t_shell *shell);
*/
bool				parse_tokens(t_shell *shell);
t_ast				*parse_pipe(t_token **token_list, t_shell *shell);
bool				is_redir(t_token_type type);
t_ast				*parse_command_and_redirection(t_token **token_list,
						t_shell *shell);
t_ast				*handle_word_ast(t_token **token_list, t_ast *cmd_node,
						t_shell *shell);
t_ast				*handle_redir_ast(t_token **token_list, t_ast **root,
						t_ast **tail, t_shell *shell);
void				append_arg(t_ast *cmd_node, const char *str,
						t_shell *shell);
void				init_arg(t_ast *cmd_node, const char *str, t_shell *shell);
char				**load_arg(char **old, size_t count, t_shell *shell);
t_ast				*add_file_node(t_token *token, t_shell *shell);
t_ast				*add_ast_node(t_token_type type, t_shell *shell);
void				move_and_free(t_token **token_list);

// split vars
void				split_variables(t_shell *shell);
void				process_split_result(t_shell *shell, t_token *current,
						char **split_result);
void				replace_token_content(t_token *current, char *new_content,
						t_shell *shell, char **split_result);
t_token				*add_expanded_token(t_token *current, t_token_type type,
						char *content);
void				free_split(char **split);
int					check_qty(char **split_result);
char				**ft_split_IFS(char *str, char *charset);

// ambiguous and validation of redirection
bool				validate_redirection(t_token *redirection);
bool    syntax_confirmed(t_token *token_list, t_shell *shell);

// errors
void				lexer_error(char *input_line, t_shell *shell);
void				free_list(t_token **list);
void				free_ast(t_ast **node);
void				free_shell_data(t_shell *shell);
void				free_shell(t_shell *shell);
void				free_array(char **arr);
void				fatality(char *msg, t_shell *shell, int exit_code);

// execute.c
void				write_bulitin_error(char *str1, char *str2, char *str3,
						char *str4);
void				error_close_and_exit(char *msg, int *pipefd);
int					get_env_size(t_env *lst);
char				*super_strjoin(char const *s1, char const *s2,
						char const *s3);
void				error_exit(char *msg);
void				recreate_env_array(t_env *env, t_shell *shell);
void				execute_cmd_child(char **args, t_shell *shell);
int					execute_external_cmd(char **args, t_shell *shell);
int					check_command(t_ast *node, char *cmd, t_shell *shell);
int					wait_children(pid_t *pids, int children_rem);
void				execute_left_child(t_ast *node, t_shell *shell,
						int *pipefd);
void				execute_right_child(t_ast *node, t_shell *shell,
						int *pipefd);
void				execute_pipe(t_ast *node, t_shell *shell);
int					execute_ast(t_ast *node, t_shell *shell);
int					get_args_len(char **args);

//-------------------builtins
int					execute_builtin_echo(char **args);
int					execute_builtin_env(char **args, t_shell *shell);
int					execute_builtin_pwd(char **args, t_shell *shell);
int					execute_builtin_cd(char **args, t_shell *shell);
int					change_working_directory(char *path, t_shell *shell);
int					execute_builtin_exit(char **args, t_shell *shell);
int					check_exit_code(char *nptr);
int					exit_numeric_error(char *nptr);
int					execute_builtin_unset(char **args, t_shell *shell);

void				process_valueless_export_node(t_shell *shell, char *str);
bool				is_identifier_valid(char *str);
void				print_env_export(t_env **temp_env);
void				bubble_sort_env(t_env **env, int len);
int					sort_and_print_export(t_env *env);
int					execute_builtin_export(char **args, t_shell *shell);

//  other exec
void				remove_env_variable(t_env **env, char *key);
void				free_env_node(t_env *env);
void				memory_error(void);
void				write_bulitin_error(char *str1, char *str2, char *str3,
						char *str4);

// redir
int					check_redirection(t_ast *ast, t_shell *shell);
#endif
