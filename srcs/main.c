/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:54:37 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/14 20:03:06 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_sig = 0;

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
		set_exec_signals();
		execute_ast(shell->ast, shell);
		free_heredoc_files(shell);
		free_shell_data(shell);
	}
	free_shell(shell);
}

/**
 * Receives and validates user input from readline.
 * Handles empty input, oversized input, and adds to history.
 *
 * @param shell Pointer to shell structure
 * @return SUCCESS if input is valid, FAILURE otherwise
 */
bool	receive_input(t_shell *shell)
{
	g_sig = 0;
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
	if (ft_strlen(shell->input_line) > INPUT_MAX)
	{
		show_error("input is too long", NULL, shell, 2);
		return (FAILURE);
	}
	add_history(shell->input_line);
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
	if (check_empty_ambig(shell) == FAILURE)
	{
		show_error("minishell: ambiguous redirect", NULL, shell, 2);
		return (FAILURE);
	}
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
