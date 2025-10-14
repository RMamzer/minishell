/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:12:40 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/14 19:46:53 by mklevero         ###   ########.fr       */
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
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <unistd.h> //open and close

extern volatile sig_atomic_t	g_sig;

# define READ_END 0
# define WRITE_END 1
# define SUCCESS 0
# define FAILURE 1
# define ALLOC true
# define NO_ALLOC false
# define EXPORT true
# define EXECUTE false
# define ENV false
# define EXIT_INVALID_OPTION 2
# define EXIT_CMD_NOT_FOUND 127
# define EXIT_CMD_NOT_EXEC 126
# define INPUT_MAX 10000
# define IN_DOUBLE_QUOTE true
# define NO_QUOTE false
# define ZERO_WORDS 0
# define ONE_WORD 1
# define ALLOC true
# define NO_ALLOC false
# define TO_TRIM " \t\n"

# define ERROR_QUOTE "syntax error: unclosed quote"
# define ERROR_MEM "cannot allocate memory"
# define ERROR_MAX_HER "maximum here-document count exceeded"
# define ERROR_EOF "warning: heredoc delimeted by EOF"
# define ERROR_NO_DIR "cd: error retrieving current directory:\
 getcwd: cannot access parent directories"
# define ERROR_NOT_F ": command not found\n"

// token type enumeration
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
}								t_token_type;

// token node struct
typedef struct s_token
{
	t_token_type				type;
	struct s_token				*next;
	char						*content;
	bool						quoted;
	bool						expanded;
}								t_token;

// environment node struct
typedef struct s_env
{
	char						*key;
	char						*value;
	struct s_env				*next;
	bool						assigned;
}								t_env;

// ast node struct
typedef struct s_ast
{
	t_token_type				type;
	char						**value;
	struct s_ast				*left;
	struct s_ast				*right;
}								t_ast;

// core shell struct
typedef struct s_shell
{
	int							exit_code;
	char						*input_line;
	t_token						*token_list;
	t_env						*env;
	t_ast						*ast;
	char						**heredoc_files;
	char						**env_array;
	char						**paths_array;
	bool						is_parent;
}								t_shell;

// main
int								main(int ac, char **av, char **env);
bool							receive_input(t_shell *shell);
bool							process_input(t_shell *shell);
bool							parse_tokens(t_shell *shell);
t_shell							*init_data(void);

// checkers
char							check_quote(char *line, int index);
bool							check_heredoc(t_shell *shell);
bool							validate_and_trim_input(t_shell *shell);
bool							check_empty_ambig(t_shell *shell);

// heredoc
bool							process_heredoc(t_shell *shell);

// heredoc_read_loop
bool							read_heredoc(int *fd, t_token *delim,
									t_shell *shell, char *file);

// heredoc_expander
void							expand_heredoc(char **line, t_shell *shell,
									char *file, int *fd);
// heredoc_save_file
void							save_heredoc_file(t_shell *shell, char *file);

// lexer
void							lexer(char *input_line, t_shell *shell);

// expander
void							expander(t_shell *shell);
char							*expand_env_var(char *content, size_t *i,
									t_env *env);
char							*get_env_value(char *name, t_env *env,
									bool alloc);

// expander_handlers
char							*handle_single_quote(char *content, size_t *i);
char							*handle_double_quote(char *content, size_t *i,
									t_shell *shell, t_token *token);
char							*handle_dollar(char *content, size_t *i,
									t_shell *shell, t_token *token);
char							*handle_characters(char *content, size_t *i,
									bool in_dq);

// confirm_syntax
bool							check_syntax(t_shell *shell);
bool							syntax_confirmed(t_token *token_list,
									t_shell *shell);

// delete_empty_tokens
void							delete_empty_tokens(t_shell *shell);

// is_helpers
bool							is_redir(t_token_type type);
bool							is_delimiter(int i);
bool							is_operator(char c);
bool							is_delim_written(char *line, t_token *delim);

// free
void							free_list(t_token **list);
void							free_ast(t_ast **ast);
void							free_env(t_env *env);
void							free_array(char **arr);
char							*strjoin_free(char *new_content, char *temp);

// free_structs
void							free_shell(t_shell *shell);
void							free_shell_data(t_shell *shell);
void							free_heredoc_files(t_shell *shell);

// exit
void							brutality(char *msg, t_shell *shell,
									int exit_code);
void							fatality(char *msg, t_shell *shell,
									int exit_code);
void							show_error(char *msg, t_token *wrong_token,
									t_shell *shell, int exit_code);
void							lexer_error(char *input_line, t_shell *shell,
									char *temp_cont);
int								ft_strcmp(const char *s1, const char *s2);

// env_creation
void							error_malloc_env_exit(char *key, char *value,
									t_shell *shell);
void							update_shllvl_value(t_shell *shell);
void							process_env_node(char *key, char *value,
									bool value_alloc, t_shell *shell);
void							process_env_line(t_shell *shell,
									const char *envp, bool process);
void							create_env(t_shell *shell, char **envp);

// env_utils
void							free_env_node(t_env *env);
bool							check_env_key(char *key, t_env *env);
void							remove_env_variable(t_env **env, char *key);
bool							update_env_value(t_env **env, char *key,
									char *new_value);
void							add_env_node(t_env **env, t_env *new_node);

// parser.c
t_ast							*parse_pipe(t_token **token_list,
									t_shell *shell);

// parser_node_creation.c
t_ast							*add_file_node(t_token *token, t_shell *shell);
t_ast							*add_ast_node(t_token_type type,
									t_shell *shell);
void							append_arg(t_ast *cmd_node, const char *str,
									t_shell *shell);

// split vars
void							split_variables(t_shell *shell);
char							**ft_split_ifs(char *str, char *charset);

// execute
int								check_command(t_ast *ast, char *cmd,
									t_shell *shell);
int								execute_ast(t_ast *ast, t_shell *shell);

// execute_utils
void							write_bulitin_error(char *str1, char *str2,
									char *str3, char *str4);
int								get_args_len(char **args);
char							*super_strjoin(char const *s1, char const *s2,
									char const *s3);
int								get_var_num(t_env *lst, bool process);
int								wait_child(pid_t pid, t_shell *shell);

// execute_cmd
void							recreate_envp_array(t_env *env, t_shell *shell);
char							*find_path_cmd(char **args, bool *malocced,
									t_shell *shell);
void							execute_cmd_child(char **args, t_shell *shell);
int								execute_external_cmd(char **args,
									t_shell *shell);

// builtin_cd
int								change_working_directory(char *path,
									t_shell *shell);
int								execute_builtin_cd(char **args, t_shell *shell);

// builtin_echo
int								execute_builtin_echo(char **args);
bool							check_echo_flag(char *str);

// builtin_env
int								execute_builtin_env(char **args,
									t_shell *shell);

////builtin_exit
int								exit_numeric_error(char *nptr);
int								process_exit_num(char *nptr);
int								check_exit_code(char *nptr);
int								execute_builtin_exit(char **args,
									t_shell *shell);

// builtin_export
void							print_env_export(t_env **temp_env);
void							bubble_sort_env(t_env **env, int len);
int								sort_and_print_export(t_env *env,
									t_shell *shell);
int								execute_builtin_export(char **args,
									t_shell *shell);
int								create_export_variables(char **args,
									t_shell *shell);
// builtin_export_utils
void							process_valueless_export_node(t_shell *shell,
									char *str);
bool							is_identifier_valid(char *str);

// builtin_pwd
int								execute_builtin_pwd(char **args,
									t_shell *shell);

// builtin_unset
int								execute_builtin_unset(char **args,
									t_shell *shell);

// pipe
int								error_close_and_return(char *msg, int *pipefd,
									int error);
int								wait_pipe(pid_t *pids, int children_rem);
void							execute_left_child(t_ast *ast, t_shell *shell,
									int *pipefd);
void							execute_right_child(t_ast *ast, t_shell *shell,
									int *pipefd);
int								execute_pipe(t_ast *ast, t_shell *shell);

// redirection
int								check_redirection(t_ast *ast, t_shell *shell);
int								execute_redirection_out(t_ast *ast,
									t_token_type type);
int								execute_redirection_in(t_ast *ast);
int								write_error_and_return(char *msg, int error);

// signals
void							set_readline_signals(void);
void							set_child_signals(void);
void							set_heredoc_signals(void);
void							set_exec_parent_signals(void);
void							set_exec_signals(void);

// signals_utils
void							signal_to_exitcode(t_shell *shell);
void							handle_readline_sigint(int sig);
void							handle_heredoc_signal(int signum);
void							handle_exe_sigint(int signum);

#endif