/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 17:56:25 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/28 17:59:00 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Determines the quote context at a specific position in the string.
 * Tracks whether the character at 'index' is inside quotes and which type.
 * 
 * @param line Input string to analyze
 * @param index Position to check quote context for
 * @return Quote character if inside quotes (', "), 0 if outside quotes
 */
char	check_quote(char *line, int index)
{
	int		i;
	char	quote;

	i = 0;
	quote = 0;
	while (i < index)
	{
		if (quote == 0 && (line[i] == '\'' || line[i] == '"'))
			quote = line[i];
		else if (quote != 0 && line[i] == quote)
			quote = 0;
		i++;
	}
	return (quote);
}

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
bool	check_pipe_syntax(t_token *current, t_shell *shell)
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
bool	check_redir_syntax(t_token *current, t_shell *shell)
{
	if (current->next == NULL)
		return (show_error(NULL, NULL, shell, 2), FAILURE);
	if (current->next->type != WORD && current->next->type != HEREDOC_DELIM_QT
		&& current->next->type != HEREDOC_DELIM_UQ)
		return (show_error(NULL, current->next, shell, 2), FAILURE);
	return (SUCCESS);
}

/**
 * Processes heredoc tokens and enforces limits.
 * Counts heredoc operators and sets delimiter types based on quoting.
 * Enforces the bash limit of 16 heredocs per command line.
 * 
 * @param shell Pointer to shell structure containing token list
 * @return SUCCESS if heredoc usage is valid, FAILURE if limit exceeded
 */
bool	check_heredoc(t_shell *shell)
{
	t_token	*current;
	int		count;

	count = 0;
	current = shell->token_list;
	while (current)
	{
		if (current->type == HEREDOC && current->next)
		{
			count++;
			if (current->next->quoted == true)
				current->next->type = HEREDOC_DELIM_QT;
			else
				current->next->type = HEREDOC_DELIM_UQ;
		}
		current = current->next;
	}
	if (count > 16)
	{
		show_error(ERROR_MAX_HER, NULL, shell, 2);
		return (FAILURE);
	}
	return (SUCCESS);
}

/**
 * Sets the 'quoted' flag for WORD tokens containing quotes.
 * This flag is used later during expansion to determine
 * whether word splitting should occur.
 * 
 * @param shell Pointer to shell structure containing token list
 */
void	quote_flag(t_shell *shell)
{
	t_token	*current;
	size_t	i;

	current = shell->token_list;
	while (current)
	{
		if (current->type == WORD)
		{
			i = 0;
			while (current->content[i])
			{
				if (current->content[i] == '\'' || current->content[i] == '"')
				{
					current->quoted = true;
					break ;
				}
				i++;
			}
		}
		current = current->next;
	}
}
