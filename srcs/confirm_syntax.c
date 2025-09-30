/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   confirm_syntax.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 14:15:15 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/30 17:04:42 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	check_pipe_syntax(t_token *current, t_shell *shell);
static bool	check_redir_syntax(t_token *current, t_shell *shell);
static bool	validate_redirection(t_token *redirection);

/**
 * Validates the overall syntax of the token list.
 * Checks for common shell syntax errors before parsing.
 * Ensures pipes and redirections have valid syntax.
 *
 * @param shell Pointer to shell structure containing token list
 * @return SUCCESS if syntax is valid, FAILURE otherwise
 */
bool	check_syntax(t_shell *shell)
{
	t_token	*current;

	if (shell->token_list == NULL)
		return (SUCCESS);
	current = shell->token_list;
	while (current)
	{
		if (current->type == PIPE && check_pipe_syntax(current,
				shell) == FAILURE)
			return (FAILURE);
		if ((current->type == IN || current->type == OUT
				|| current->type == APPEND || current->type == HEREDOC)
			&& check_redir_syntax(current, shell) == FAILURE)
			return (FAILURE);
		current = current->next;
	}
	return (SUCCESS);
}

/**
 * Validates pipe operator syntax.
 * Pipes cannot be at the beginning of command, at the end,
 * or followed by another pipe
 *
 * @param current Current pipe token to validate
 * @param shell Pointer to shell structure for error reporting
 * @return SUCCESS if pipe syntax is valid, FAILURE otherwise
 */
static bool	check_pipe_syntax(t_token *current, t_shell *shell)
{
	if (current == shell->token_list)
		return (show_error(NULL, current, shell, 2), FAILURE);
	if (current->next == NULL)
		return (show_error(NULL, NULL, shell, 2), FAILURE);
	if (current->next->type == PIPE)
		return (show_error(NULL, current->next, shell, 2), FAILURE);
	return (SUCCESS);
}

/**
 * Validates redirection operator syntax.
 * Redirection operators must be followed by a filename (WORD token)
 * or heredoc delimiter.
 *
 * @param current Current redirection token to validate
 * @param shell Pointer to shell structure for error reporting
 * @return SUCCESS if redirection syntax is valid, FAILURE otherwise
 */
static bool	check_redir_syntax(t_token *current, t_shell *shell)
{
	if (current->next == NULL)
		return (show_error(NULL, NULL, shell, 2), FAILURE);
	if (current->next->type != WORD && current->next->type != HEREDOC_DELIM_QT
		&& current->next->type != HEREDOC_DELIM_UQ)
		return (show_error(NULL, current->next, shell, 2), FAILURE);
	return (SUCCESS);
}

/**
 * Confirms the syntax validity of the token list before parsing.
 * Check for ambiguous redirections that could cause parsing errors.
 *
 * @param token_list The list of tokens to validate
 * @param shell Pointer to shell structure for error reporting
 * @return SUCCESS if syntax is valid, FAILURE otherwise
 */
bool	syntax_confirmed(t_token *token_list, t_shell *shell)
{
	t_token	*current;

	current = token_list;
	while (current)
	{
		if (is_redir(current->type) && !validate_redirection(current))
		{
			show_error("minishell: ambiguous redirect", NULL, shell, 2);
			return (FAILURE);
		}
		current = current->next;
	}
	return (SUCCESS);
}

/**
 * Validates redirection tokens to detect ambiguous redirection.
 * An ambiguous redirect occurs when an expanded variable results is
 * multiple words or none, where only one filename is expected.
 *
 * @param redirection The redirection token to validate
 * @return true if redirection is valid, false if ambiguous
 */
static bool	validate_redirection(t_token *redirection)
{
	t_token	*token;

	if (!redirection || !redirection->next)
		return (false);
	token = redirection->next;
	if (token->expanded && token->next && token->next->type == WORD)
		return (false);
	return (true);
}
