/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_structs.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 17:25:55 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/10 15:05:04 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Completely frees the shell structure and all associated data.
 * Called when exiting the minishell entirely.
 * Frees both per-command data and persistent shell data.
 *
 * @param shell Pointer to shell structure
 */
void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	free_shell_data(shell);
	if (shell->env)
		free_env(shell->env);
	free(shell);
}

/**
 * Frees shell data structures for the current command cycle.
 * Cleans up input line, token list, AST, and execution arrays.
 * Prepares shell for the next command without freeing persistent data.
 *
 * @param shell Pointer to shell structure
 */
void	free_shell_data(t_shell *shell)
{
	if (!shell)
		return ;
	free(shell->input_line);
	shell->input_line = NULL;
	free_list(&shell->token_list);
	free_ast(&shell->ast);
	if (shell->is_parent == true)
		free_heredoc_files(shell);
	else if (shell->heredoc_files)
		free_array(shell->heredoc_files);
	free_array(shell->env_array);
	shell->env_array = NULL;
	free_array(shell->paths_array);
	shell->paths_array = NULL;
}

/**
 * Cleans up all heredoc temporary files.
 * Removes files from filesystem and frees the tracking array.
 * Called at the end of command execution.
 *
 * @param shell Pointer to shell structure
 */
void	free_heredoc_files(t_shell *shell)
{
	size_t	i;

	if (!shell->heredoc_files)
		return ;
	i = 0;
	while (shell->heredoc_files[i])
	{
		unlink(shell->heredoc_files[i]);
		free(shell->heredoc_files[i]);
		i++;
	}
	free(shell->heredoc_files);
	shell->heredoc_files = NULL;
}
