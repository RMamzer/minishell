/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:54:37 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/01 17:45:08 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_sig = 0;

void	signal_to_exitcode(t_shell *shell)
{
	static t_shell	*sig_shell;

	if (shell)
		sig_shell = shell;
	else if (g_sig != 0)
	{
		sig_shell->exit_code = g_sig + 128;
	}
	g_sig = 0;
}
/**
 * Signal handler for SIGINT during readline input.
 * Provides clean interrupt behavior when Ctrl+C is pressed.
 * Updates display and sets signal for exit code.
 *
 * @param sig Signal number received
 */
void	handle_readline_sigint(int sig)
{
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	g_sig = sig;
	signal_to_exitcode(NULL);
}
/**
 * Signal handler for SIGINT during heredoc input.
 * Handles interruption of heredoc reading.
 *
 * @param signum Signal number received
 */
void	handle_heredoc_signal(int signum)
{
	g_sig = signum;
	signal_to_exitcode(NULL);
}

void	handle_sigint_exe(int signum)
{
	(void)signum;
	write(1, "\n", 1);
}

/**
 * Sets up signal handling for readline input phase.
 * Configures appropriate signal behavior for interactive input.
 */
void	set_readline_signals(void)
{
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, handle_readline_sigint);
}

/**
 * Sets default signal handling for child processes.
 * Restores default signal behavior for executed commands.
 */
void	child_signal(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
/**
 * Sets up signal handling for heredoc input phase.
 * Configures signals for heredoc reading with special behavior.
 */
void	set_heredoc_signal(void)
{
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, handle_heredoc_signal);
}

//  void	restore_main_signals(void)
//  {
//  	signal(SIGQUIT, SIG_IGN);
//  	signal(SIGINT, handle_sigint_exe);
// }

/**
 * Sets signal handling during command execution preparation.
 * Ignores signals while setting up execution environment.
 */
void	set_signals_exec(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}
/**
 * Sets signal handling for parent process during command execution.
 * Provides appropriate signal handling while waiting for children.
 */
void	set_signals_exec_parent(void)
{
	signal(SIGINT, handle_sigint_exe);
	signal(SIGQUIT, SIG_IGN);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Main entry point for the minishell program.
 *
 * @param ac Argument count (unused)
 * @param av Argument vector (unused)
 * @param env Environment variables array
 */
int	main(int ac, char **av, char **env)
{
	t_shell	*shell;

	(void)ac;
	(void)av;
	shell = init_data();
	create_env(shell, env);
	while (1)
	{
		if (receive_input(shell) == FAILURE)
			continue ;
		if (process_input(shell) == FAILURE)
			continue ;
		if (parse_tokens(shell) == FAILURE)
			continue ;
		set_signals_exec();
		execute_ast(shell->ast, shell);
		free_heredoc_files(shell);
		free_shell_data(shell);
	}
	free_shell(shell);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Receives and validates user input from readline.
 * Handles empty input, oversized input, and adds to history.
 *
 * @param shell Pointer to shell structure
 * @return SUCCESS if input is valid, FAILURE otherwise
 */
bool	receive_input(t_shell *shell)
{
	set_readline_signals();
	shell->input_line = readline("dirty_shell> ");
	if (!shell->input_line)
	{
		ft_putstr_fd("exit\n", STDOUT_FILENO);
		fatality(NULL, shell, 0);
	}
	if (shell->input_line[0] == '\0')
	{
		free(shell->input_line);
		return (FAILURE);
	}
	if (ft_strlen(shell->input_line) > 1024) // for now
	{
		show_error("input is too long", NULL, shell, 2);
		return (FAILURE);
	}
	add_history(shell->input_line);
	return (SUCCESS);
}

/**
 * Validates and trims whitespace from input.
 * Checks for unclosed quotes and prepares input for lexing.
 *
 * @param shell Pointer to shell structure
 * @return SUCCESS if input is valid, FAILURE otherwise
 */
bool	validate_and_trim_input(t_shell *shell)
{
	char	*line;

	line = ft_strtrim(shell->input_line, TO_TRIM);
	if (line == NULL || line[0] == '\0')
	{
		free(shell->input_line);
		free(line);
		return (FAILURE);
	}
	if (check_quote(line, ft_strlen(line)) != 0)
	{
		free(line);
		show_error(ERROR_QUOTE, NULL, shell, 2);
		return (FAILURE);
	}
	free(shell->input_line);
	shell->input_line = line;
	return (SUCCESS);
}

/**
 * Main input processing pipeline.
 * Prepares raw input for executable AST.
 *
 * @param shell Pointer to shell structure
 * @return SUCCESS if processing completes, FAILURE otherwise
 */
bool	process_input(t_shell *shell)
{
	if (validate_and_trim_input(shell) == FAILURE)
		return (FAILURE);
	lexer(shell->input_line, shell);
	if (check_syntax(shell) == FAILURE)
		return (FAILURE);
	if (check_heredoc(shell) == FAILURE)
		return (FAILURE);
	if (process_heredoc(shell) == FAILURE)
		return (free_shell_data(shell), FAILURE);
	expander(shell);
	split_variables(shell);
	delete_empty_tokens(shell);
	return (SUCCESS);
}

/**
 * Main parsing entry point. Validates syntax and builds the AST.
 *
 * @param shell Pointer to the shell structure containing token list
 * @return SUCCESS if parsing succeeds, FAILURE otherwise
 */
bool	parse_tokens(t_shell *shell)
{
	if (!shell || !shell->token_list)
		return (FAILURE);
	if (syntax_confirmed(shell->token_list, shell) == FAILURE)
		return (FAILURE);
	shell->ast = parse_pipe(&shell->token_list, shell);
	return (SUCCESS);
}

/**
 * Initializes the main shell data structure.
 * Allocates memory and sets up initial state for shell operation.
 *
 * @return Pointer to initialized shell structure
 */
t_shell	*init_data(void)
{
	t_shell	*shell;

	shell = ft_calloc(1, sizeof(t_shell));
	if (shell == NULL)
	{
		ft_putendl_fd(ERROR_MEM, 2);
		exit(FAILURE);
	}
	shell->is_parent = true;
	signal_to_exitcode(shell);
	return (shell);
}
