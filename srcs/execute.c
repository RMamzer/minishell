/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/10/08 22:28:53 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Verifies if cmd is a build-in or external command and starts
 * its execution.
 * @param ast The ast containing right side of the pipe execution.
 * @param cmd Command being processed.
 * @param shell Pointer to the shell struct.
 * @return Exit status of the command execution.
 */
int	check_command(t_ast *ast, char *cmd, t_shell *shell)
{
	if (!*cmd)
		shell->exit_code = 0;
	else if (ft_strcmp(cmd, "echo") == 0)
		shell->exit_code = execute_builtin_echo(ast->value + 1);
	else if (ft_strcmp(cmd, "cd") == 0)
		shell->exit_code = execute_builtin_cd(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "pwd") == 0)
		shell->exit_code = execute_builtin_pwd(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "export") == 0)
		shell->exit_code = execute_builtin_export(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "unset") == 0)
		shell->exit_code = execute_builtin_unset(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "env") == 0)
		shell->exit_code = execute_builtin_env(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "exit") == 0)
		shell->exit_code = execute_builtin_exit(ast->value + 1, shell);
	else
		shell->exit_code = execute_external_cmd(ast->value, shell);
	return (shell->exit_code);
}

/**
 * Starts the recursion executing pipes, redirections, and cmds from ast.
 * @param ast The ast node being analyzed (1st on the initial run).
 * @param shell Pointer to the shell struct.
 * @return Exit status of the execution sequence.
 */
int	execute_ast(t_ast *ast, t_shell *shell)
{
	if (!ast || !shell)
		return (0);
	if (ast->type == WORD)
		shell->exit_code = check_command(ast, ast->value[0], shell);
	if (ast->type == PIPE)
		shell->exit_code = execute_pipe(ast, shell);
	else if (ast->type >= IN && ast->type <= APPEND)
		shell->exit_code = check_redirection(ast, shell);
	return (shell->exit_code);
}